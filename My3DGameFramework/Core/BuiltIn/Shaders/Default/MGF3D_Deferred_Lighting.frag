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

// --------------------------------------------------------
// Lighting Functions (포워드 렌더링과 동일한 공식 적용)
// --------------------------------------------------------
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float specFactor, float shininess)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * albedo;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), max(shininess, 1.0));
    vec3 specular = light.color.rgb * spec * specFactor;

    return (diffuse + specular) * light.color.a;
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
    
    return (diffuse + specular) * attenuation * light.color.a;
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
    
    return (diffuse + specular) * attenuation * intensity * light.color.a;
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
        resultColor += CalcDirLight(dirLights[i], normal, viewDir, albedo, specFactor, shininess);

    for(int i = 0; i < uScene.pointLightCount; ++i)
        resultColor += CalcPointLight(pointLights[i], normal, fragPos, viewDir, albedo, specFactor, shininess);

    for(int i = 0; i < uScene.spotLightCount; ++i)
        resultColor += CalcSpotLight(spotLights[i], normal, fragPos, viewDir, albedo, specFactor, shininess);

    // 4. 에미션 적용
    resultColor += emission;

    FragColor = vec4(resultColor, 1.0);
}