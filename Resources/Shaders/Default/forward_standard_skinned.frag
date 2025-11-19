#version 460 core

in vec2 texCoords;
in vec3 normal;
in vec3 position;
in vec4 FragPosLightSpace;

out vec4 fragColor;

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
uniform vec3 viewPos;

// C++ (Material::SetToProgram)에서 바인딩할 디퓨즈 텍스처
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
    float emissionStrength;
};
uniform Material material;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 1. Perspective Divide (직교 투영이면 w가 1이라 변화 없음)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // 2. [-1, 1] 범위를 [0, 1] 범위로 변환
    projCoords = projCoords * 0.5 + 0.5;

    // 4. 현재 픽셀의 깊이와 바이어스 계산 (Shadow Acne 방지)
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);

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
    // Ambient
    vec3 texColor = texture(material.diffuse, texCoords).xyz;
    vec3 ambient = texColor * light.ambient;

    // Attenuation and Diffuse
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - position) / dist;

    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp
    (
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
        0.0, 1.0
    );

    if (intensity > 0.0) 
    {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        // blinn-phong's specular operation
        vec3 specColor = texture(material.specular, texCoords).xyz;
        vec3 viewDir = normalize(viewPos - position);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        vec3 specular = spec * specColor * light.specular;

        float shadow = ShadowCalculation(FragPosLightSpace, pixelNorm, lightDir);

        result += (1.0 - shadow) * (diffuse + specular) * intensity;
    }

    result *= attenuation;

    // emission
    vec3 emission = texture(material.emission, texCoords).rgb * material.emissionStrength;
    result += emission;

    fragColor = vec4(result, 1.0);
}