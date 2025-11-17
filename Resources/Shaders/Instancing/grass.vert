#version 460 core

// Per-Vertex (StaticMesh)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aInstanceData; 

out vec3 FragPos_world;
out vec3 Normal_world;
out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // --- 1. 회전 행렬 생성 ---
    float angle = aInstanceData.y; // Y축 회전값
    float c = cos(angle);
    float s = sin(angle);
    // Y축 회전 행렬
    mat4 rotMat = mat4(
        c, 0.0,  s, 0.0,
      0.0, 1.0, 0.0, 0.0,
       -s, 0.0,  c, 0.0,
      0.0, 0.0, 0.0, 1.0
    );

    vec3 rotatedPos = (rotMat * vec4(aPos, 1.0)).xyz;
    vec3 finalPos = rotatedPos + vec3(aInstanceData.x, 0.0, aInstanceData.z);
    
    gl_Position = projection * view * vec4(finalPos, 1.0);
    
    FragPos_world = finalPos;
    Normal_world = (rotMat * vec4(aNormal, 0.0)).xyz;
    texCoord = aTexCoord;
}