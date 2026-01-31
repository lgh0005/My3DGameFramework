#version 460 core

layout (location = 0) in vec3 aPos;

struct StaticInstanceProperty { mat4 worldMatrix; };

layout (std430, binding = 0) readonly buffer InstanceBuffer 
{
    StaticInstanceProperty instances[];
};

uniform mat4 lightSpaceMatrix;

void main() 
{
    mat4 model = instances[gl_InstanceID].worldMatrix;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}