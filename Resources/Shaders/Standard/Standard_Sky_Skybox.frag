#version 460 core

in vec3 texCoord;
layout (location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main() 
{
    vec3 color = texture(skybox, texCoord).rgb;
    fragColor = vec4(color * 1.5, 1.0);
}