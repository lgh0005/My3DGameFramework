#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoord;

// --------------------------------------------------------
// G-Buffer Samplers
// --------------------------------------------------------
layout(binding = 0) uniform sampler2D gPositionAO;
layout(binding = 1) uniform sampler2D gNormalRoughness;
layout(binding = 2) uniform sampler2D gAlbedoMetallic;
layout(binding = 3) uniform sampler2D gEmission;
// layout(binding = 4) uniform sampler2D gVelocity;
layout(binding = 5) uniform sampler2D gSSAO;
layout(binding = 6) uniform sampler2DArray gShadowMapDir;
layout(binding = 7) uniform samplerCubeArray gShadowMapPoint;
layout(binding = 8) uniform sampler2DArray gShadowMapSpot;

layout(binding = 9) uniform samplerCube irradianceMap;
layout(binding = 10) uniform samplerCube prefilterMap;
layout(binding = 11) uniform sampler2D brdfLUT;

// --------------------------------------------------------
// Scene & Light Uniforms
// --------------------------------------------------------
layout(std140, binding = 0) uniform SceneGlobalBuffer
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
    float time;
    int dirLightCount;
    int pointLightCount;
    int spotLightCount;
    int pad1;
} uScene;

const float PI = 3.14159265359;

struct DirectionalLight { vec4 direction; vec4 color; int shadowIndex; int pad1; int pad2; int pad3; };
struct PointLight       { vec4 position;  vec4 color; int shadowIndex; int pad1; int pad2; int pad3; };
struct SpotLight        { vec4 position;  vec4 direction; vec4 color; vec4 params; int shadowIndex; int pad1; int pad2; int pad3; };

layout(std430, binding = 3) readonly buffer DirLightBuffer   { DirectionalLight dirLights[]; };
layout(std430, binding = 4) readonly buffer PointLightBuffer { PointLight pointLights[]; };
layout(std430, binding = 5) readonly buffer SpotLightBuffer  { SpotLight spotLights[]; };

struct DirShadowData { mat4 lightSpaceMatrices[4]; vec4 cascadeSplits; int shadowMapBaseIdx; float shadowBias; int pad0; int pad1; };
struct PointShadowData { float shadowFarPlane; int shadowMapIdx; float shadowBias; int pad0; };
struct SpotShadowData { mat4 lightSpaceMatrix; int shadowMapIdx; float shadowBias; int pad0; int pad1; };

layout(std430, binding = 6) readonly buffer DirShadowBuffer   { DirShadowData dirShadows[]; };
layout(std430, binding = 7) readonly buffer PointShadowBuffer { PointShadowData pointShadows[]; };
layout(std430, binding = 8) readonly buffer SpotShadowBuffer  { SpotShadowData spotShadows[]; };

// --------------------------------------------------------
// 3. Shadow Calculation Functions
// --------------------------------------------------------
// Directional Light (CSM) 그림자 계산
float CalcDirShadow(int shadowIndex, vec3 fragPosWorld, vec3 normal, vec3 lightDir)
{
    if (shadowIndex < 0) return 0.0;
    DirShadowData sData = dirShadows[shadowIndex];

    // 1. 현재 프래그먼트의 View 공간 Z값 추출
    vec4 fragPosView = uScene.view * vec4(fragPosWorld, 1.0);
    float depthValue = abs(fragPosView.z);
    
    // 2. 4개의 Cascade 중 어디에 속하는지 판별
    int layer = 3;
    for (int i = 0; i < 4; ++i) 
    {
        if (depthValue < sData.cascadeSplits[i]) 
        {
            layer = i;
            break;
        }
    }
    
    // 3. 해당 층의 행렬로 프래그먼트를 빛의 시점으로 투영
    vec4 fragPosLightSpace = sData.lightSpaceMatrices[layer] * vec4(fragPosWorld, 1.0);
    if (fragPosLightSpace.w <= 0.0) return 0.0;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // NDC [-1, 1] -> [0, 1]
    
    float currentDepth = projCoords.z;
    float bias = max(sData.shadowBias * 5.0 * (1.0 - dot(normal, normalize(-lightDir))), sData.shadowBias);
    
    // 4. PCF (3x3) 샘플링으로 그림자 경계선 블러링
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(gShadowMapDir, 0).xy;
    float targetLayer = float(sData.shadowMapBaseIdx + layer);
    
    for(int x = -1; x <= 1; ++x) 
    {
        for(int y = -1; y <= 1; ++y) 
        {
            float pcfDepth = texture(gShadowMapDir, vec3(projCoords.xy + vec2(x, y) * texelSize, targetLayer)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

// Point Light 그림자 계산
float CalcPointShadow(int shadowIndex, vec3 fragPosWorld, vec3 lightPos)
{
    if (shadowIndex < 0) return 0.0;
    PointShadowData sData = pointShadows[shadowIndex];

    vec3 fragToLight = fragPosWorld - lightPos;
    float currentDepth = length(fragToLight); // 월드 공간 실제 거리
    
    float shadow = 0.0;
    float bias = sData.shadowBias;
    float farPlane = sData.shadowFarPlane;
    float targetLayer = float(sData.shadowMapIdx);

    // PCF를 위한 20개의 샘플링 방향 벡터 (성능과 퀄리티 타협)
    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    
    int samples = 20;
    float diskRadius = (1.0 + (length(uScene.viewPos - fragPosWorld) / farPlane)) / 25.0;
    
    for(int i = 0; i < samples; ++i)
    {
        // 텍스처에서 가져온 값을 farPlane을 곱해 월드 공간 선형 깊이로 복원
        float closestDepth = texture(gShadowMapPoint, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, targetLayer)).r;
        closestDepth *= farPlane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    return shadow / float(samples);
}

// Spot Light 그림자 계산
float CalcSpotShadow(int shadowIndex, vec3 fragPosWorld, vec3 normal, vec3 lightDir)
{
    if (shadowIndex < 0) return 0.0;
    SpotShadowData sData = spotShadows[shadowIndex];

    vec4 fragPosLightSpace = sData.lightSpaceMatrix * vec4(fragPosWorld, 1.0);
    if (fragPosLightSpace.w <= 0.0) return 0.0;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(sData.shadowBias * 5.0 * (1.0 - dot(normal, normalize(lightDir))), sData.shadowBias);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(gShadowMapSpot, 0).xy;
    float targetLayer = float(sData.shadowMapIdx);
    
    for(int x = -1; x <= 1; ++x) 
    {
        for(int y = -1; y <= 1; ++y) 
        {
            float pcfDepth = texture(gShadowMapSpot, vec3(projCoords.xy + vec2(x, y) * texelSize, targetLayer)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

// --------------------------------------------------------
// PBR Functions
// --------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --------------------------------------------------------
// Lighting Functions (포워드 렌더링과 동일한 공식 적용)
// --------------------------------------------------------
vec3 CalcPBRLight(vec3 lightDir, vec3 lightColor, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0)
{
    vec3 H = normalize(viewDir + lightDir);
    float NDF = DistributionGGX(normal, H, roughness);   
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
       
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    float NdotL = max(dot(normal, lightDir), 0.0);        
    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specFactor, float shininess)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = light.color.rgb * spec * specFactor;

    float shadow = CalcDirShadow(light.shadowIndex, fragPos, normal, lightDir);

    return (diffuse + specular) * (1.0 - shadow) * light.color.a;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specFactor, float shininess)
{
    vec3 lightVector = light.position.xyz - fragPos;
    float distance = length(lightVector);
    float range = light.position.w;
    
    if(distance > range) return vec3(0.0);
    
    vec3 lightDir = normalize(lightVector);
    float attenuation = clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0);
    attenuation *= attenuation;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = light.color.rgb * spec * specFactor;
    
    float shadow = CalcPointShadow(light.shadowIndex, fragPos, light.position.xyz);

    return (diffuse + specular) * attenuation * (1.0 - shadow) * light.color.a;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specFactor, float shininess)
{
    vec3 lightVector = light.position.xyz - fragPos;
    float distance = length(lightVector);
    float range = light.position.w;
    
    if(distance > range) return vec3(0.0);
    
    vec3 lightDir = normalize(lightVector);
    float attenuation = clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0);
    attenuation *= attenuation;

    float theta = dot(lightDir, normalize(-light.direction.xyz)); 
    float epsilon = max(light.params.x - light.params.y, 0.0001);
    float intensity = clamp((theta - light.params.y) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = light.color.rgb * spec * specFactor;
    
    float shadow = CalcSpotShadow(light.shadowIndex, fragPos, normal, lightDir);

    return (diffuse + specular) * attenuation * intensity * (1.0 - shadow) * light.color.a;
}

void main()
{
    // 1. G-Buffer 데이터 언패킹 (Unpacking)
    vec4 dataPosAO      = texture(gPositionAO, vTexCoord);
    vec4 dataNormRough  = texture(gNormalRoughness, vTexCoord);
    vec4 dataAlbMetal   = texture(gAlbedoMetallic, vTexCoord);
    vec4 dataEmission   = texture(gEmission, vTexCoord);

    vec3 fragPos    = dataPosAO.xyz;
    float ao        = dataPosAO.a * texture(gSSAO, vTexCoord).r;
    vec3 normal     = normalize(dataNormRough.xyz);
    
    // 만약 배경(빈 공간)이라면 렌더링하지 않음
    if (dot(normal, normal) < 0.1) 
        discard;

    // 2. PBR 파라미터 및 공통 계산 
    // (이제 shininess, specFactor라는 단어는 엔진에서 완전히 퇴출되었습니다!)
    vec3 viewDir    = normalize(uScene.viewPos - fragPos);
    vec3 albedo     = dataAlbMetal.rgb;
    float roughness = clamp(dataNormRough.a, 0.05, 1.0); // 0.0 방지용 최소값 0.05
    float metallic  = clamp(dataAlbMetal.a,  0.0,  1.0);
    vec3 emission   = dataEmission.rgb;

    // 기본 반사율 계산
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 R = reflect(-viewDir, normal);

    // IBL (Image-Based Lighting) Ambient 연산 시작
    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    // a. Diffuse IBL (Irradiance Map)
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse    = irradiance * albedo;

    // b. Specular IBL (Prefilter Map + BRDF LUT)
    const float MAX_REFLECTION_LOD = 4.0; // 생성된 프리필터 맵의 최대 Mip 단계
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    // 최종 Ambient (주변광) 도출
    vec3 ambient = (kD * diffuse + specularIBL) * ao;

    vec3 resultColor = ambient;
     for (int i = 0; i < uScene.dirLightCount; ++i)
    {
        vec3 lightDir = normalize(-dirLights[i].direction.xyz);
        float shadow  = CalcDirShadow(dirLights[i].shadowIndex, fragPos, normal, lightDir);
        vec3 radiance = dirLights[i].color.rgb * dirLights[i].color.a;
        resultColor  += CalcPBRLight(lightDir, radiance, normal, viewDir, albedo, roughness, metallic, F0)
                        * (1.0 - shadow);
    }

    for (int i = 0; i < uScene.pointLightCount; ++i)
    {
        vec3 lightVec = pointLights[i].position.xyz - fragPos;
        float dist    = length(lightVec);
        float range   = pointLights[i].position.w;
        if (dist > range) continue;

        vec3  lightDir  = normalize(lightVec);
        float atten     = clamp(1.0 - (dist * dist) / (range * range), 0.0, 1.0);
        atten          *= atten;
        float shadow    = CalcPointShadow(pointLights[i].shadowIndex, fragPos, pointLights[i].position.xyz);
        vec3  radiance  = pointLights[i].color.rgb * pointLights[i].color.a * atten;
        resultColor    += CalcPBRLight(lightDir, radiance, normal, viewDir, albedo, roughness, metallic, F0)
                          * (1.0 - shadow);
    }

    for (int i = 0; i < uScene.spotLightCount; ++i)
    {
        vec3  lightVec  = spotLights[i].position.xyz - fragPos;
        float dist      = length(lightVec);
        float range     = spotLights[i].position.w;
        if (dist > range) continue;

        vec3  lightDir  = normalize(lightVec);
        float atten     = clamp(1.0 - (dist * dist) / (range * range), 0.0, 1.0);
        atten          *= atten;
        float theta     = dot(lightDir, normalize(-spotLights[i].direction.xyz));
        float epsilon   = max(spotLights[i].params.x - spotLights[i].params.y, 0.0001);
        float intensity = clamp((theta - spotLights[i].params.y) / epsilon, 0.0, 1.0);
        float shadow    = CalcSpotShadow(spotLights[i].shadowIndex, fragPos, normal, lightDir);
        vec3  radiance  = spotLights[i].color.rgb * spotLights[i].color.a * atten * intensity;
        resultColor    += CalcPBRLight(lightDir, radiance, normal, viewDir, albedo, roughness, metallic, F0)
                          * (1.0 - shadow);
    }

    // 5. emission
    resultColor += emission;

    FragColor = vec4(resultColor, 1.0);
}