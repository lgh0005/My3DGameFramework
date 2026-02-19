#version 460 core
in vec4 vOutlineColor;
layout (location = 0) out vec4 FragColor;

void main() 
{
    FragColor = vOutlineColor; // 마스크 텍스처에 아웃라인 색상 기록
}