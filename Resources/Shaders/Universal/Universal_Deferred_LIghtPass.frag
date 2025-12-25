#version 460 core
out vec4 FragColor;

in vec2 TexCoords; // Screen Quad UV

uniform sampler2D gPosition;    // RGB: WorldPos, A: Material AO
uniform sampler2D gNormal;      // RGB: Normal,   A: Roughness
uniform sampler2D gAlbedoSpec;  // RGB: Albedo,   A: Metallic
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
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // [-1,1] -> [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    // Far plane 넘어가는 것 예외처리
    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    // Shadow Acne 방지용 Bias
    float bias = max(0.00025 * (1.0 - dot(normal, lightDir)), 0.000025);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    // PCF (3x3 Sampling)
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

// --- Shadow Map Selector ---
// GLSL 배열 인덱싱 문제 해결을 위한 분기문
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

// --- PBR Math Functions ---
float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) 
{
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) 
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return dotNV / (dotNV * (1.0 - k) + k);
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
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) 
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // 1. G-Buffer Unpacking
    vec4 posSample = texture(gPosition, TexCoords);
    vec3 FragPos   = posSample.rgb;
    float MatAO    = posSample.a; // Material AO (ORM.r)

    vec4 normSample = texture(gNormal, TexCoords);
    vec3 N          = normSample.rgb;
    float Roughness = normSample.a; // Roughness (ORM.g)

    vec4 albedoSample = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo       = albedoSample.rgb;
    float Metallic    = albedoSample.a; // Metallic (ORM.b)

    vec3 Emission = texture(gEmission, TexCoords).rgb;

    // SSAO Sampling & Combine
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

        if (lights[i].type == 0) // Directional
        {
            L = normalize(-lights[i].direction);
        }
        else // Point & Spot
        {
            L = normalize(lights[i].position - FragPos);
            float dist = length(lights[i].position - FragPos);
            attenuation = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);
        }

        if (lights[i].type == 2) // Spot Intensity
        {
             float theta = dot(L, normalize(-lights[i].direction));
             float epsilon = lights[i].cutoff.x - lights[i].cutoff.y;
             float spotIntensity = clamp((theta - lights[i].cutoff.y) / epsilon, 0.0, 1.0);
             attenuation *= spotIntensity;
        }

        // Shadow Calculation
        float shadow = 0.0;
        int shadowIdx = lights[i].shadowMapIndex;
        // 유효한 그림자 인덱스일 때만 계산
        if (shadowIdx >= 0 && shadowIdx < 8) 
        {
            vec4 lightSpacePos = lightSpaceMatrices[shadowIdx] * vec4(FragPos, 1.0);
            shadow = GetShadowFactor(shadowIdx, lightSpacePos, N, L); 
        }

        // Cook-Torrance BRDF
        vec3 H = normalize(V + L);
        vec3 radiance = lights[i].diffuse * lights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, Roughness);   
        float G   = GeometrySmith(N, V, L, Roughness);      
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - Metallic);       

        float NdotL = max(dot(N, L), 0.0);        
        
        // Final Additive Lighting
        Lo += (1.0 - shadow) * (kD * Albedo / PI + specular) * radiance * NdotL;
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
        
        float iblStrength = 1.0;
        ambient = (kD * diffuse + specular) * FinalAO * iblStrength;
    }
    else
    {
        // Fallback Ambient
        ambient = vec3(0.03) * Albedo * FinalAO; 
    }

    // 5. Final Mix
    vec3 color = ambient + Lo + Emission;

    FragColor = vec4(color, 1.0);
}