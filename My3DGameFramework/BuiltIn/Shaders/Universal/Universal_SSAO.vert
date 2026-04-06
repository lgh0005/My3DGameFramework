#version 460 core

// StaticMesh::Init에서 정의한 속성 순서와 일치해야 합니다.
layout (location = 0) in vec3 aPos;       // Position
layout (location = 2) in vec2 aTexCoords; // TexCoord (1번 Normal은 사용 안 함)

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}