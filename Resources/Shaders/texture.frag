#version 460 core

in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() 
{
    fragColor = texture(tex1, texCoord) * 0.8 + texture(tex2, texCoord) * 0.2;
}