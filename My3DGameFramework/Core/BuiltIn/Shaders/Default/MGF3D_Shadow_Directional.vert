#version 460 core

// 메쉬의 기본 정점 데이터
layout(location = 0) in vec3 aPos;

// RenderQueue에서 넘겨주는 인스턴스 데이터 (bindingSlot = 1 이라고 가정)
struct InstanceData 
{
    mat4 worldMatrix;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer 
{
    InstanceData instances[];
};

void main() 
{
    // 1. 현재 인스턴스의 월드 행렬을 가져옴
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    
    // 2. 월드 공간 좌표로 변환하여 지오메트리 셰이더로 전달
    gl_Position = worldMat * vec4(aPos, 1.0); 
}