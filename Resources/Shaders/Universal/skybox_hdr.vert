#version 460 core

layout (location = 0) in vec3 aPos;
out vec3 localPos;

uniform mat4 transform;

void main() 
{
    localPos = aPos;
    vec4 pos = transform * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}