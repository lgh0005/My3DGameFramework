#version 460 core

in vec2 TexCoords;
in vec3 normal;
in vec3 position;

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
    float shininess;
};

// [수정] 2. 'material'이라는 이름으로 struct 유니폼을 선언합니다.
uniform Material material;

void main()
{
    // Ambient
    vec3 texColor = texture(material.diffuse, TexCoords).xyz;
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
        vec3 specColor = texture(material.specular, TexCoords).xyz;
        vec3 viewDir = normalize(viewPos - position);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);

        vec3 specular = spec * specColor * light.specular;

        result += (diffuse + specular) * intensity;
    }
    result *= attenuation;
    fragColor = vec4(result, 1.0);

    // 텍스처 로딩 실패 시 디버깅용 코드 (선택 사항)
    if (fragColor.a < 0.1)
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}