#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;    
// TEMP : UBO 테스트 중
// uniform mat4 lightSpaceMatrix;
layout (std140, binding = 2) uniform ShadowData
{
    mat4 lightSpaceMatrix; 
};

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}