#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;   // 1번을 Normal로!
layout(location = 2) in vec2 aTexCoord; // 2번을 UV로!
layout(location = 3) in vec3 aTangent;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vWorldPos;
layout(location = 2) out vec3 vNormal;

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
    vec4 worldPos = worldMat * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vTexCoord = aTexCoord;

    // 노멀 계산
    vNormal = mat3(worldMat) * aNormal;
    
    // 최종 클립 공간 좌표 계산 (P * V * M * Pos)
    gl_Position = uCamera.projection * uCamera.view * worldPos;
 }