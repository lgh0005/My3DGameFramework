#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    vec3 color = vec3(TexCoord.x, TexCoord.y, 1.0 - TexCoord.x);
    FragColor = vec4(color, 1.0);
}