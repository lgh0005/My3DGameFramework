#version 460 core

// 출력
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

// [G-Buffer]
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmission;

// [Shadow]
// 아직 단일 텍스처 사용 (나중에 Array로 변경 시 이 부분만 고치면 됨)
uniform sampler2D shadowMap; 
// 그림자 행렬은 UBO가 아니라 유니폼으로 받기로 했었나요? 
// 아, Scene::PreRender에서 shadowMapIndex=0을 줬으니, 여기서 행렬을 어떻게 받을지가 관건입니다.
// 지금은 일단 "그림자 끄기" 모드로 갈 것이므로, 행렬 변수는 선언만 해두거나 주석 처리합니다.
uniform mat4 lightSpaceMatrix; 

// [Light UBO]
struct LightInfo
{
    vec3 position;      float pad0;
    vec3 direction;     float pad1;
    vec2 cutoff;        vec2  pad2;
    vec3 attenuation;   float pad3;
    vec3 ambient;       float pad4;
    vec3 diffuse;       float pad5;
    vec3 specular;      float pad6;
    int type;           
    int shadowMapIndex;
    vec2 pad7;
};

#define MAX_LIGHTS 32

layout (std140, binding = 1) uniform LightData
{
    LightInfo lights[MAX_LIGHTS];
    vec3 viewPos;
    int lightCount;
};

// [그림자 계산]
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
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

// [조명 계산 함수]
vec3 CalcLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specInt, float shininess)
{
    LightInfo light = lights[index];

    // 1. 방향 벡터 계산
    vec3 lightDir;
    if (light.type == 0) // Directional
        lightDir = normalize(-light.direction);
    else 
        lightDir = normalize(light.position - fragPos);

    // 2. Diffuse & Specular 기본값
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), shininess);

    // 3. Attenuation (감쇠)
    float attenuation = 1.0;
    if (light.type != 0) // Point or Spot
    {
        float dist = length(light.position - fragPos);
        attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);
    }

    // 4. Spot Intensity
    float intensity = 1.0;
    if (light.type == 2) // Spot
    {
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutoff.x - light.cutoff.y; // inner - outer
        intensity = clamp((theta - light.cutoff.y) / epsilon, 0.0, 1.0);
    }

    // 5. 그림자
    float shadow = 0.0;
    if (light.shadowMapIndex >= 0) 
    {
        vec4 lightSpacePos = lightSpaceMatrix * vec4(fragPos, 1.0);
        shadow = ShadowCalculation(lightSpacePos, normal, lightDir);
    }
   
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * specInt;

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation * intensity;
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec4 AlbedoData = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = AlbedoData.rgb;
    float SpecInt = AlbedoData.a;
    vec3 Emission = texture(gEmission, TexCoords).rgb;
    float Shininess = texture(gNormal, TexCoords).a; 

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for(int i = 0; i < lightCount; ++i)
    {
        result += CalcLight(i, Normal, FragPos, viewDir, Albedo, SpecInt, Shininess);
    }
    
    result += Emission;
    FragColor = vec4(result, 1.0);
    
    // Bloom...
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) BrightColor = vec4(result, 1.0);
    else BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}