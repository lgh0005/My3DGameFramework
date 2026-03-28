#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(TexCoord.x, TexCoord.y, 1.0 - TexCoord.x, 1.0);
}