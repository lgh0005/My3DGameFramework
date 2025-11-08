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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
	vec3 ambient = material.ambient * light.ambient;

	// Diffuse
	vec3 lightDir = normalize(light.position - position);
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;
	
	// Specular
	vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

	// Result
	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0);
}