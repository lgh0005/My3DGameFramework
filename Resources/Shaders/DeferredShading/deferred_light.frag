#version 460 core

// 출력: 0번은 최종 색상(HDR), 1번은 밝은 색상(Bloom용)
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

// [G-Buffer]
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmission;

// [Shadow]
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;
uniform vec3 viewPos;

// [Light]
struct Light 
{
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 1. Perspective Divide (직교 투영이면 w가 1이라 변화 없음)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // 2. [-1, 1] 범위를 [0, 1] 범위로 변환
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) 
        return 0.0;

    // 4. 현재 픽셀의 깊이와 바이어스 계산 (Shadow Acne 방지)
    float currentDepth = projCoords.z;
    float bias = max(0.00025 * (1.0 - dot(normal, lightDir)), 0.000025);

    // 5. PCF (Percentage-Closer Filtering) - 부드러운 그림자
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

void main()
{
    // 1. G-Buffer 데이터 복원
    // [Position] .rgb만 가져옵니다.
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    // [Normal + Shininess]
    vec4 NormalData = texture(gNormal, TexCoords);
    vec3 Normal = normalize(NormalData.rgb);
    float Shininess = NormalData.a;

    // [Albedo + Specular Intensity]
    vec4 albedoData = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = albedoData.rgb;
    float SpecularIntensity = albedoData.a;

    // [Emission]
    vec3 Emission = texture(gEmission, TexCoords).rgb;

    // 2. 조명 연산 준비
    // Ambient
    vec3 ambient = Albedo * light.ambient;

    // Attenuation and Diffuse 
    float dist = length(light.position - FragPos);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - FragPos) / dist;

    vec3 result = ambient;

    // Spotlight Intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp
    (
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
        0.0, 1.0
    );

    if (intensity > 0.0) 
    {
        // Diffuse
        // G-Buffer의 Normal을 바로 사용
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * Albedo * light.diffuse;

        // Specular (Blinn-Phong)
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        
        float spec = pow(max(dot(Normal, halfDir), 0.0), Shininess);
        vec3 specular = spec * SpecularIntensity * light.specular;

        // Shadow Calculation
        vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
        float shadow = ShadowCalculation(fragPosLightSpace, Normal, lightDir);

        result += (1.0 - shadow) * (diffuse + specular) * intensity;
    }

    result *= attenuation;
    result += Emission;

    // 최종 HDR 색상
    FragColor = vec4(result, 1.0);

    // Bloom용 밝은 색상 추출
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) // HDR 임계값 (취향에 따라 0.4 ~ 1.0 조절)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}