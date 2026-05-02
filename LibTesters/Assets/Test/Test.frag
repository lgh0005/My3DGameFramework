#version 460 core

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vWorldPos;
layout(location = 2) in vec3 vNormal;

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

// --------------------------------------------------------
// 조명 구조체 (std430 정렬에 맞추어 패딩 수정)
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

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    
    // Diffuse 연산
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    return diffuse * light.color.a; // 색상 * 강도
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    // 1. 조명을 향하는 벡터 및 거리 계산
    vec3 lightVector = light.position.xyz - fragPos;
    float distance = length(lightVector);
    float range = light.position.w; // w 성분에 저장된 Range
    
    // 2. 조명 범위를 벗어나면 연산 제외 (최적화)
    if(distance > range) return vec3(0.0);
    
    vec3 lightDir = normalize(lightVector);
    
    // 3. 거리 감쇠 (Attenuation) - 부드럽게 빛이 사라지는 언리얼/유니티 방식
    float attenuation = clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0);
    attenuation *= attenuation; // 자연스러운 곡선을 위해 제곱

    // 4. Diffuse (난반사)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    // 5. Specular (정반사 - Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = vec3(1.0) * spec * albedoFactor.a; // 임시 스펙큘러 강도
    
    // 최종 조합 (색상 * 거리감쇠 * 강도)
    return (diffuse + specular) * attenuation * light.color.a;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightVector = light.position.xyz - fragPos;
    float distance = length(lightVector);
    float range = light.position.w;
    
    if(distance > range) return vec3(0.0);
    
    vec3 lightDir = normalize(lightVector);
    
    // 1. 거리 감쇠 (Point Light와 동일)
    float attenuation = clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0);
    attenuation *= attenuation;

    // 2. 스포트라이트 각도 감쇠 (Cone Intensity)
    // light.direction은 빛이 나아가는 방향, lightDir은 프래그먼트에서 빛을 향하는 방향이므로 음수(-) 처리
    float theta = dot(lightDir, normalize(-light.direction.xyz)); 
    float epsilon = light.params.x - light.params.y; // x: innerCutoff, y: outerCutoff
    float intensity = clamp((theta - light.params.y) / epsilon, 0.0, 1.0);

    // 3. Diffuse & Specular
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = vec3(1.0) * spec * albedoFactor.a;
    
    // 최종 조합 (색상 * 거리감쇠 * 각도감쇠 * 강도)
    return (diffuse + specular) * attenuation * intensity * light.color.a;
}

void main() 
{
    // 1. 머티리얼 알베도 결정
    vec4 texColor = texture(uAlbedoMap, vTexCoord);
    vec3 albedo = texColor.rgb * albedoFactor.rgb;

    // 2. 필요 벡터 계산
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(uScene.viewPos - vWorldPos);

    // 3. 환경광 (아주 어두운 기본 밝기, 아예 검은색을 피하기 위함)
    vec3 resultColor = albedo * 0.05;

    // 4. Directional Lights 누적
    for(int i = 0; i < uScene.dirLightCount; ++i)
    {
        resultColor += CalcDirLight(dirLights[i], norm, viewDir, albedo);
    }

    // 5. Point Lights 누적
    for(int i = 0; i < uScene.pointLightCount; ++i)
    {
        resultColor += CalcPointLight(pointLights[i], norm, vWorldPos, viewDir, albedo);
    }

    // 6. Spot Lights 누적
    for(int i = 0; i < uScene.spotLightCount; ++i)
    {
        resultColor += CalcSpotLight(spotLights[i], norm, vWorldPos, viewDir, albedo);
    }

    // 최종 출력 (텍스처의 알파값 유지)
    FragColor = vec4(resultColor, texColor.a * albedoFactor.a);
}