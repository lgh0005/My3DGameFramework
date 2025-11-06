#version 460 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() 
{
    vec4 color = texture(tex, texCoord);
    fragColor = color * vertexColor;
}