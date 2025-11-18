#version 460 core

////////////////////////////////////////////////
// 조명 컴포넌트의 동작을 확인할 임시 셰이더 코드 //
////////////////////////////////////////////////

in vec3 normal;
in vec2 texCoord;
in vec3 position;

uniform vec3 viewPos;

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

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

out vec4 fragColor;

// TODO
// 1. 광원에 따른 셰이더 필요
void main() 
{
	// Ambient
	vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
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

        vec3 specColor = texture2D(material.specular, texCoord).xyz;

        // blinn-phong's specular operation
        vec3 viewDir = normalize(viewPos - position);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);

        vec3 specular = spec * specColor * light.specular;

        result += (diffuse + specular) * intensity;
    }
    result *= attenuation;
	fragColor = vec4(result, 1.0);
}