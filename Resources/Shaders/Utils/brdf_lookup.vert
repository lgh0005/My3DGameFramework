#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

// uniform mat4 transform; // <--- 이거 주석 처리 또는 무시

out vec2 texCoord;

void main() 
{
    // 행렬 곱셈 없이 그대로 전달 (NDC 좌표 -1~1을 직접 넣을 거니까)
    gl_Position = vec4(aPos, 1.0); 
    texCoord = aTexCoord;
}