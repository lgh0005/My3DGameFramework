#version 460	

layout (location = 0) in vec3 pos;	
layout (location = 1) in vec3 color;

out vec3 vertexColor;
uniform mat4 model;
									
void main()			
{						
	gl_Position = model * vec4(pos.x, pos.y, pos.z, 1.0); 
	vertexColor = color;
}