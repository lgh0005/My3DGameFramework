#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vWorldPos;
layout(location = 2) out mat3 vTBN;

// [SSBO] 인스턴싱 데이터
struct InstanceData
{
    mat4 worldMatrix;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer 
{
    InstanceData instances[];
};

// [UBO] 글로벌 통합 데이터 (Slot 0)
layout(std140, binding = 0) uniform SceneGlobalBuffer
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
    float time;
    int dirLightCount;
    int pointLightCount;
    int spotLightCount;
    int pad1;
} uScene;

void main()
{
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    vec4 worldPos = worldMat * vec4(aPos, 1.0);
    
    vWorldPos = worldPos.xyz;
    vTexCoord = aTexCoord;
    
    // 비균등 스케일을 고려한 노멀 행렬 계산
    mat3 normalMatrix = transpose(inverse(mat3(worldMat)));

    // TBN 벡터 월드 공간 변환
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);

    // 글로벌 데이터의 프로젝션, 뷰 행렬 사용
    gl_Position = uScene.projection * uScene.view * worldPos;
}