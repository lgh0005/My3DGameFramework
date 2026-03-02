#version 460 core
layout (location = 0) in vec3 aPos;   // Position
layout (location = 1) in vec4 aColor; // Color

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

out vec4 vColor;

void main()
{
    vColor = aColor;
    gl_Position = projection * view * vec4(aPos, 1.0);
}