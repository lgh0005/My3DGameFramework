#version 460 core
layout(location = 0) in vec3 aPos;

// [SSBO] C++에서 보낸 인스턴싱 데이터
struct InstanceData
{
    mat4 worldMatrix;
};

// [SSBO] std430 레이아웃으로 바인딩 슬롯 1번과 연결
layout(std430, binding = 1) readonly buffer InstanceBuffer 
{
    InstanceData instances[];
};

// [UBO] C++에서 보낸 카메라 데이터 (std140, Slot 0)
layout(std140, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
    float pad0;
} uCamera;

void main()
{
   // SSBO에서 현재 인스턴스의 월드 행렬 가져오기
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    
    // UBO에서 받은 행렬들을 조합하여 ViewProjection 행렬 계산
    mat4 viewProj = uCamera.projection * uCamera.view;
    
    // 최종 클립 공간 좌표 계산 (P * V * M * Pos)
    gl_Position = viewProj * worldMat * vec4(aPos, 1.0);
 }