#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vWorldPos;
layout(location = 2) out mat3 vTBN;
layout(location = 5) out vec4 vCurrClipPos;

// [SSBO] 정적 메쉬 인스턴싱 데이터
struct StaticInstanceData { mat4 worldMatrix; };
layout(std430, binding = 1) readonly buffer InstanceBuffer { StaticInstanceData instances[]; };

// [UBO] 글로벌 통합 데이터
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

// 카메라의 이전 뷰-프로젝션 행렬 (렌더 컨텍스트에서 넣어주어야 함)
uniform mat4 uPrevVP; 

void main()
{
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    vec4 worldPos = worldMat * vec4(aPos, 1.0);
    
    vWorldPos = worldPos.xyz;
    vTexCoord = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(worldMat)));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);

    // 벨로시티(모션 벡터)를 위한 클립 공간 좌표 계산
    vCurrClipPos = uScene.projection * uScene.view * worldPos;
    gl_Position = vCurrClipPos;
}