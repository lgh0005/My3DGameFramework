#version 460 core
out vec4 FragColor;

in vec2 TexCoords; // Screen Quad UV

uniform sampler2D gPosition;    // RGB: WorldPos, A: Material AO
uniform sampler2D gNormal;      // RGB: Normal,    A: Roughness
uniform sampler2D gAlbedoSpec;  // RGB: Albedo,    A: Metallic
uniform sampler2D gEmission;    // RGB: Emission

uniform sampler2D ssaoTexture;
uniform sampler2D shadowMaps[8];

layout(binding = 13) uniform samplerCube irradianceMap;
layout(binding = 14) uniform samplerCube prefilterMap;
layout(binding = 15) uniform sampler2D   brdfLUT;

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
};

struct LightInfo
{
    vec3 position;    float pad0;
    vec3 direction;   float pad1;
    vec2 cutoff;      vec2  pad2;
    vec3 attenuation; float pad3;
    vec3 ambient;     float pad4;
    vec3 diffuse;     float intensity;
    vec3 specular;    float pad5;
    int type;         int shadowMapIndex; vec2 pad6;
};

#define MAX_LIGHTS 32
layout (std140, binding = 1) uniform LightData
{
    LightInfo lights[MAX_LIGHTS];
    vec3 lightPos;
    int lightCount;
};

uniform int useIBL;
uniform mat4 lightSpaceMatrices[8];

const float PI = 3.14159265359;

// --- Shadow Calculation (PCF) ---
float CalculateShadow(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.00025 * (1.0 - dot(normal, lightDir)), 0.000025);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

float GetShadowFactor(int index, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    if (index == 0) return CalculateShadow(shadowMaps[0], fragPosLightSpace, normal, lightDir);
    if (index == 1) return CalculateShadow(shadowMaps[1], fragPosLightSpace, normal, lightDir);
    if (index == 2) return CalculateShadow(shadowMaps[2], fragPosLightSpace, normal, lightDir);
    if (index == 3) return CalculateShadow(shadowMaps[3], fragPosLightSpace, normal, lightDir);
    if (index == 4) return CalculateShadow(shadowMaps[4], fragPosLightSpace, normal, lightDir);
    if (index == 5) return CalculateShadow(shadowMaps[5], fragPosLightSpace, normal, lightDir);
    if (index == 6) return CalculateShadow(shadowMaps[6], fragPosLightSpace, normal, lightDir);
    if (index == 7) return CalculateShadow(shadowMaps[7], fragPosLightSpace, normal, lightDir);
    return 0.0;
}

// --- PBR Math Functions (Safety Guarded) ---
float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) 
{
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    // [Safety] 분모 0 방지
    return a2 / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float dotNV, float roughness) 
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom    = dotNV;
    float denom = dotNV * (1.0 - k) + k;
    // [Safety] 분모 0 방지
    return nom / max(denom, 0.0000001);
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) 
{
    float dotNV = max(dot(normal, viewDir), 0.0);
    float dotNL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) 
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) 
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// [Safety] NaN 검사 함수
bool IsExploded(vec3 v)
{
    return isnan(v.x) || isnan(v.y) || isnan(v.z) || isinf(v.x) || isinf(v.y) || isinf(v.z);
}

void main()
{
    // [Debug Flag] NaN 발생 여부 추적
    bool isExploded = false;

    // 1. G-Buffer Unpacking
    vec4 posSample = texture(gPosition, TexCoords);
    vec3 FragPos   = posSample.rgb;
    float MatAO    = posSample.a; 

    vec4 normSample = texture(gNormal, TexCoords);
    vec3 N          = normSample.rgb;
    // [Safety] Roughness 최소값 강제 (가장 중요)
    float Roughness = max(normSample.a, 0.05); 

    vec4 albedoSample = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo       = albedoSample.rgb;
    float Metallic    = albedoSample.a; 

    vec3 Emission = texture(gEmission, TexCoords).rgb;

    float SSAO = texture(ssaoTexture, TexCoords).r;
    float FinalAO = MatAO * SSAO;

    // 2. Setup PBR Vector
    vec3 V = normalize(camPos - FragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, Albedo, Metallic);

    vec3 Lo = vec3(0.0);

    // 3. Direct Light Loop
    for(int i = 0; i < lightCount; ++i)
    {
        // Calculate Light Direction & Attenuation
        vec3 L;
        float attenuation = 1.0;
        float dist = 0.0;

        if (lights[i].type == 0) // Directional
        {
            L = normalize(-lights[i].direction);
            dist = 1.0; 
        }
        else // Point & Spot
        {
            vec3 lightVec = lights[i].position - FragPos;
            dist = length(lightVec);
            
            // [Safety] 광원과 너무 가까우면(거리 0) 계산 스킵
            if(dist < 0.001) continue;

            L = normalize(lightVec);
            
            // [Safety] 감쇠 공식 분모 0 방지
            float attenDenom = lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist;
            attenuation = 1.0 / max(attenDenom, 0.0001);
        }

        if (lights[i].type == 2) // Spot Intensity
        {
             float theta = dot(L, normalize(-lights[i].direction));
             float epsilon = lights[i].cutoff.x - lights[i].cutoff.y;
             // [Safety] 엡실론 0 방지
             float spotIntensity = clamp((theta - lights[i].cutoff.y) / max(epsilon, 0.0001), 0.0, 1.0);
             attenuation *= spotIntensity;
        }

        // Shadow Calculation
        float shadow = 0.0;
        int shadowIdx = lights[i].shadowMapIndex;
        if (shadowIdx >= 0 && shadowIdx < 8) 
        {
            vec4 lightSpacePos = lightSpaceMatrices[shadowIdx] * vec4(FragPos, 1.0);
            shadow = GetShadowFactor(shadowIdx, lightSpacePos, N, L); 
        }

        // Cook-Torrance BRDF
        vec3 H;
        vec3 lightDirPlusViewDir = V + L;
        // [Safety] Half Vector 안전 정규화
        if (dot(lightDirPlusViewDir, lightDirPlusViewDir) < 1e-6)
            H = N;
        else
            H = normalize(lightDirPlusViewDir);

        vec3 radiance = lights[i].diffuse * lights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, Roughness);   
        float G   = GeometrySmith(N, V, L, Roughness);      
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F; 
        
        // [Safety] Specular 분모 0 방지
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - Metallic);        

        float NdotL = max(dot(N, L), 0.0);        
        
        // 각 라이트별 결과 계산
        vec3 lightResult = (1.0 - shadow) * (kD * Albedo / PI + specular) * radiance * NdotL;

        // [Debug] 결과가 NaN인지 확인
        if (IsExploded(lightResult))
        {
            isExploded = true;
        }
        else
        {
            Lo += lightResult;
        }
    }

    // 4. Ambient (IBL)
    vec3 ambient = vec3(0.0);
    if (useIBL > 0) 
    {
        vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, Roughness);
        vec3 kD = 1.0 - kS;
        kD *= (1.0 - Metallic);
        
        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse    = irradiance * Albedo;
        
        const float MAX_REFLECTION_LOD = 4.0; 
        vec3 prefilteredColor = textureLod(prefilterMap, R, Roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), Roughness)).rg;
        vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
        
        float iblStrength = 0.4;
        ambient = (kD * diffuse + specular) * FinalAO * iblStrength;
    }
    else
    {
        // Fallback Ambient
        ambient = vec3(0.03) * Albedo * FinalAO; 
    }

    // [Debug] IBL 계산 결과도 NaN인지 확인 (옵션)
    if (IsExploded(ambient)) isExploded = true;

    // 5. Final Mix
    vec3 color = ambient + Lo + Emission;

    // [최종 디버그 출력]
    if (isExploded)
    {
        // 수학적 오류(NaN/Inf) 발생 시 핫핑크 출력
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); 
    }
    else
    {
        // 정상 출력
        FragColor = vec4(color, 1.0);
    }
}