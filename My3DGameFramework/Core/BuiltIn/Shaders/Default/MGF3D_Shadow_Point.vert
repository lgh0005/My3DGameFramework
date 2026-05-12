#version 460 core

layout(location = 0) in vec3 aPos;

// RenderQueue에서 넘겨주는 인스턴스 데이터
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
    // 월드 공간 좌표로 변환하여 지오메트리 셰이더로 전달
    gl_Position = instances[gl_InstanceID].worldMatrix * vec4(aPos, 1.0); 
}