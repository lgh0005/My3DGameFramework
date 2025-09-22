#version 460	

in vec2 TexCoord; 
out vec4 FragColor;

uniform sampler2D textureSampler; 

void main()
{
	FragColor = texture(textureSampler, TexCoord);
}