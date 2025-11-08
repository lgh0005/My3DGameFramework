#version 460 core

in vec3 normal;
in vec2 texCoord;
in vec3 position;

uniform vec3 viewPos;

struct Light 
{
    vec3 position;
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
// 2. blinn-phong 모델로 수정해야 함.
void main() 
{
	// Ambient
	vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

	// Diffuse
	vec3 lightDir = normalize(light.position - position);
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;
	
	// Specular
    vec3 specColor = texture2D(material.specular, texCoord).xyz;
	vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specColor * light.specular;

	// Result
	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0);
}