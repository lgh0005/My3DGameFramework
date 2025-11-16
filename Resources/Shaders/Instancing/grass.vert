#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aInstanceOffset; 

out vec3 FragPos_world;
out vec3 Normal_world;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec3 worldPos = aPos + aInstanceOffset;   
    gl_Position = projection * view * vec4(worldPos, 1.0);
    
    FragPos_world = worldPos;
    Normal_world = aNormal;
    TexCoord = aTexCoord;
}