#version 460 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;

void main() 
{
    vec3 envColor = texture(cubeMap, localPos).rgb;
    fragColor = vec4(envColor, 1.0);
}