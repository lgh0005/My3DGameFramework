#version 460 core

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vWorldPos;
layout(location = 2) in mat3 vTBN;
layout(location = 5) in vec4 vCurrClipPos;

layout(location = 0) out vec4 gPositionAO;
layout(location = 1) out vec4 gNormalRoughness;
layout(location = 2) out vec4 gAlbedoMetallic;
layout(location = 3) out vec4 gEmission;
layout(location = 4) out vec2 gVelocity;

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

layout(std140, binding = 2) uniform MaterialBlock 
{
    vec4  albedoFactor;

    vec3  emissiveFactor;
    float emissionStrength; 

    float heightScale;
    float metallicFactor;
    float roughnessFactor;
    float pad;
};

layout(binding = 0) uniform sampler2D uAlbedoMap;
layout(binding = 1) uniform sampler2D uSpecularMap;
layout(binding = 2) uniform sampler2D uEmissionMap;
layout(binding = 3) uniform sampler2D uNormalMap;
layout(binding = 4) uniform sampler2D uHeightMap;
layout(binding = 5) uniform sampler2D uORMMap;

void main() 
{
    // 1. Albedo
    vec4 texColor = texture(uAlbedoMap, vTexCoord);
    vec3 albedo = texColor.rgb * albedoFactor.rgb;
    if (texColor.a * albedoFactor.a < 0.1) discard;

    // 2. Normal
    vec3 normalSample = texture(uNormalMap, vTexCoord).rgb;
    normalSample = normalize(normalSample * 2.0 - 1.0);
    vec3 N = normalize(vTBN * normalSample);

    // 3. PBR 파라미터 구성
    vec3 orm = texture(uORMMap, vTexCoord).rgb;
    float ao        = orm.r; 
    float roughness = orm.g;
    float metallic  = orm.b;

    // 4. Emission
    vec3 emission = texture(uEmissionMap, vTexCoord).rgb * emissiveFactor * emissionStrength;

    // 5. G-Buffer 출력
    gPositionAO      = vec4(vWorldPos, ao);
    gNormalRoughness = vec4(N, roughness);
    gAlbedoMetallic  = vec4(albedo, metallic);
    gEmission        = vec4(emission, 1.0);
    gVelocity = vec2(0.0);
}