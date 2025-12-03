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

// [Shadow] - 배열로 변경!
uniform sampler2D shadowMaps[8]; 
uniform mat4 lightSpaceMatrices[8]; // 행렬도 배열로!

// [SSAO sampler]
uniform sampler2D ssaoTexture;
uniform bool useSSAO;

// [Light UBO]
struct LightInfo
{
    vec3 position;      float pad0;
    vec3 direction;     float pad1;
    vec2 cutoff;        vec2  pad2;
    vec3 attenuation;   float pad3;
    vec3 ambient;       float pad4;
    vec3 diffuse;       float intensity;
    vec3 specular;      float pad5;
    int type;            
    int shadowMapIndex; vec2 pad6;
};

#define MAX_LIGHTS 32

layout (std140, binding = 1) uniform LightData
{
    LightInfo lights[MAX_LIGHTS];
    vec3 viewPos;
    int lightCount;
};

// [그림자 계산 핵심 로직] 
// 텍스처를 인자로 받아서 처리하도록 분리
float CalculateShadow(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // 그림자 맵 범위 밖이면 그림자 없음
    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
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

// [그림자 맵 선택기]
// Sampler Array Indexing 문제 방지를 위한 안전한 Switch문
float GetShadowFactor(int index, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 인덱스에 따라 올바른 텍스처를 넘겨줌
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

// [조명 계산 함수]
// TODO : 조명 타입에 맞춰서 코드를 좀 더 명확히 써서 
// 타입별로 어떻게 연산이 되는 것인지 조금은 명확히 작성 필요
vec3 LightCalculation(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specInt, float shininess, float ao)
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

    // 4. Spot Factor (이름 변경 주의: intensity -> spotFactor)
    float spotFactor = 1.0;
    if (light.type == 2) // Spot
    {
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutoff.x - light.cutoff.y;
        spotFactor = clamp((theta - light.cutoff.y) / epsilon, 0.0, 1.0);
    }

    // 5. 그림자 계산 (핵심 수정)
    float shadow = 0.0;
    int shadowIdx = light.shadowMapIndex;

    // 인덱스가 유효할 때만 계산
    if (shadowIdx >= 0 && shadowIdx < 8) 
    {
        // 행렬 배열에서 가져옴
        vec4 lightSpacePos = lightSpaceMatrices[shadowIdx] * vec4(fragPos, 1.0);
        
        // 텍스처 선택기를 통해 그림자 값 계산
        shadow = GetShadowFactor(shadowIdx, lightSpacePos, normal, lightDir);
    }
   
    vec3 ambient = light.ambient * albedo * ao;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * specInt;

    // light.intensity 곱해주는 것 잊지 말기!
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation * spotFactor * light.intensity;
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

    float ao = 1.0; // 기본값: 차폐 없음
    if (useSSAO)
    {
        // 텍스처의 r 채널에 occlusion 값이 들어있음 (1.0 = 흰색 = 열림, 0.0 = 검은색 = 막힘)
        ao = texture(ssaoTexture, TexCoords).r;
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for(int i = 0; i < lightCount; ++i)
    {
        result += LightCalculation(i, Normal, FragPos, viewDir, Albedo, SpecInt, Shininess, ao);
    }
    
    result += Emission;
    // =====================================================================
    // [DEBUG] SSAO 적용 결과 확인용 코드
    // =====================================================================
    // 원래의 최종 결과 출력 코드를 주석 처리합니다.
    FragColor = vec4(result, 1.0);
    
    // 대신 ao 값을 R, G, B 채널에 모두 넣어서 흑백으로 출력합니다.
    // useSSAO가 true일 때: 구석진 곳은 검은색(0.0), 트인 곳은 흰색(1.0)으로 나와야 성공입니다.
    // useSSAO가 false일 때: 전체가 흰색(1.0)으로 나와야 합니다.
    // vec3 debugPos = texture(gPosition, TexCoords).rgb;
    // FragColor = vec4(vec3(ao), 1.0); 

    // =====================================================================
    
    // Bloom Threshold
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) BrightColor = vec4(result, 1.0);
    else BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

