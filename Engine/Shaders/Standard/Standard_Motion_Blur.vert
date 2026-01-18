#version 460 core

layout (location = 0) in vec3 aPos;       // Quad Vertex Position
layout (location = 2) in vec2 aTexCoords; // Quad UV

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}