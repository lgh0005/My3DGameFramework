#version 460 core

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vWorldPos;
layout(location = 2) in mat3 vTBN;

layout(location = 0) out vec4 FragColor;

// [UBO] 글로벌 통합 데이터 (Slot 0)
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

// [UBO] 머티리얼 데이터 (Slot 2)
layout(std140, binding = 2) uniform MaterialBlock 
{
    vec4  albedoFactor;
    vec3  emissiveFactor;
    float emissionStrength; 
    float shininess;
    float heightScale;
    float metallicFactor;
    float roughnessFactor;
};

layout(binding = 0) uniform sampler2D uAlbedoMap;
layout(binding = 1) uniform sampler2D uSpecularMap;
layout(binding = 2) uniform sampler2D uEmissionMap;
layout(binding = 3) uniform sampler2D uNormalMap;
layout(binding = 4) uniform sampler2D uHeightMap;
layout(binding = 5) uniform sampler2D uORMMap;

// --------------------------------------------------------
// 조명 구조체
// --------------------------------------------------------
struct DirectionalLight 
{
    vec4 direction;
    vec4 color;
    int  shadowIndex;
    int  pad1; int pad2; int pad3;
};

struct PointLight 
{
    vec4 position;
    vec4 color;
    int  shadowIndex;
    int  pad1; int pad2; int pad3;
};

struct SpotLight 
{
    vec4 position;
    vec4 direction;
    vec4 color;
    vec4 params;
    int  shadowIndex;
    int  pad1; int pad2; int pad3;
};

layout(std430, binding = 3) readonly buffer DirLightBuffer   { DirectionalLight dirLights[]; };
layout(std430, binding = 4) readonly buffer PointLightBuffer { PointLight pointLights[]; };
layout(std430, binding = 5) readonly buffer SpotLightBuffer  { SpotLight spotLights[]; };

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float specFactor)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = light.color.rgb * spec * specFactor;

    return (diffuse + specular) * light.color.a;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specFactor)
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
    
    return (diffuse + specular) * attenuation * light.color.a;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specFactor)
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
    
    return (diffuse + specular) * attenuation * intensity * light.color.a;
}

void main() 
{
    // 노멀맵 텍스처를 그대로 화면에 출력해봅니다.
    vec3 testMap = texture(uNormalMap, vTexCoord).rgb;
    FragColor = vec4(testMap, 1.0);
    return;

    // 1. 머티리얼 속성 추출
    vec4 texColor = texture(uAlbedoMap, vTexCoord);
    vec3 albedo = texColor.rgb * albedoFactor.rgb;
    float specFactor = texture(uSpecularMap, vTexCoord).r;

    // 2. 노멀 맵 적용 (TBN 변환)
    vec3 normalSample = texture(uNormalMap, vTexCoord).rgb;
    normalSample = normalize(normalSample * 2.0 - 1.0); // [-1, 1] 범위로 매핑
    vec3 norm = normalize(vTBN * normalSample);

    // 3. 뷰 방향 및 기본 환경광 설정
    vec3 viewDir = normalize(uScene.viewPos - vWorldPos);
    vec3 resultColor = albedo * 0.05; // Ambient

    // 4. 조명 누적 계산
    for(int i = 0; i < uScene.dirLightCount; ++i)
        resultColor += CalcDirLight(dirLights[i], norm, viewDir, albedo, specFactor);

    for(int i = 0; i < uScene.pointLightCount; ++i)
        resultColor += CalcPointLight(pointLights[i], norm, vWorldPos, viewDir, albedo, specFactor);

    for(int i = 0; i < uScene.spotLightCount; ++i)
        resultColor += CalcSpotLight(spotLights[i], norm, vWorldPos, viewDir, albedo, specFactor);

    // 5. 에미션(발광) 추가
    vec3 emission = texture(uEmissionMap, vTexCoord).rgb * emissiveFactor * emissionStrength;
    resultColor += emission;

    // 6. 최종 출력
    FragColor = vec4(resultColor, texColor.a * albedoFactor.a);
}