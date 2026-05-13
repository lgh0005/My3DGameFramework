#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoord;

// --------------------------------------------------------
// G-Buffer Samplers
// --------------------------------------------------------
layout(binding = 0) uniform sampler2D gPositionAO;
layout(binding = 1) uniform sampler2D gNormalRoughness; // .a 채널에 shininess 저장됨
layout(binding = 2) uniform sampler2D gAlbedoMetallic;  // .a 채널에 specFactor 저장됨
layout(binding = 3) uniform sampler2D gEmission;
// layout(binding = 4) uniform sampler2D gVelocity;
layout(binding = 5) uniform sampler2D gSSAO;
layout(binding = 6) uniform sampler2DArray gShadowMapDir;
layout(binding = 7) uniform samplerCubeArray gShadowMapPoint;
layout(binding = 8) uniform sampler2DArray gShadowMapSpot;

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
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // NDC [-1, 1] -> [0, 1]
    
    if (projCoords.z > 1.0) return 0.0;
    
    float currentDepth = projCoords.z;
    // 경사에 따른 가변 바이어스 (피터팬 현상 방지)
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
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0; // 조명 밖은 그림자 없음

    float currentDepth = projCoords.z;
    float bias = max(sData.shadowBias * 5.0 * (1.0 - dot(normal, normalize(-lightDir))), sData.shadowBias);
    
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
// Lighting Functions (포워드 렌더링과 동일한 공식 적용)
// --------------------------------------------------------
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
    float epsilon = light.params.x - light.params.y;
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
    vec3 normal     = dataNormRough.xyz;
    
    // G-Buffer의 Alpha 채널에서 기존 방식의 파라미터를 꺼내옵니다.
    float shininess = dataNormRough.a;
    vec3 albedo     = dataAlbMetal.rgb;
    float specFactor= dataAlbMetal.a;
    vec3 emission   = dataEmission.rgb;

    // 만약 배경(빈 공간)이라면 렌더링하지 않음
    if (dot(normal, normal) < 0.1) 
        discard;

    // 2. 공통 계산
    vec3 viewDir = normalize(uScene.viewPos - fragPos);
    vec3 ambient = albedo * 0.05 * ao;
    vec3 resultColor = ambient;

    // 3. 조명 누적 계산 (인자 수정 반영)
    for(int i = 0; i < uScene.dirLightCount; ++i)
        resultColor += CalcDirLight(dirLights[i], normal, fragPos, viewDir, albedo, specFactor, shininess);

    for(int i = 0; i < uScene.pointLightCount; ++i)
        resultColor += CalcPointLight(pointLights[i], normal, fragPos, viewDir, albedo, specFactor, shininess);

    for(int i = 0; i < uScene.spotLightCount; ++i)
        resultColor += CalcSpotLight(spotLights[i], normal, fragPos, viewDir, albedo, specFactor, shininess);

    // 4. 에미션 적용
    resultColor += emission;

    FragColor = vec4(resultColor, 1.0);
}