#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in ivec4 aBoneIDs; 
layout(location = 5) in vec4 aWeights;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vWorldPos;
layout(location = 2) out mat3 vTBN;

// [SSBO] 스킨드 메쉬 인스턴싱 데이터
struct SkinnedInstanceData
{
    mat4 worldMatrix;
    uint boneOffset;
    uint pad1;
    uint pad2;
    uint pad3;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer 
{
    SkinnedInstanceData instances[];
};

// [SSBO] 글로벌 뼈대 버퍼
layout(std430, binding = 9) readonly buffer BoneBuffer
{
    mat4 uBones[];
};

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

void main()
{
    // 1. 인스턴스 데이터 추출
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    uint boneOffset = instances[gl_InstanceID].boneOffset;

    // 2. 스키닝 행렬(Skin Matrix) 계산
    mat4 boneTransform = mat4(0.0);
    for(int i = 0; i < 4; ++i)
    {
        if(aBoneIDs[i] >= 0)
        {
            uint finalBoneIndex = boneOffset + uint(aBoneIDs[i]);
            boneTransform += uBones[finalBoneIndex] * aWeights[i];
        }
    }

    // 3. 로컬 공간 변환 (애니메이션 적용)
    vec4 localPos = boneTransform * vec4(aPos, 1.0);
    vec3 localNormal = mat3(boneTransform) * aNormal;
    vec3 localTangent = mat3(boneTransform) * aTangent;

    // 4. 월드 공간 변환
    vec4 worldPos = worldMat * localPos;
    vWorldPos = worldPos.xyz;
    vTexCoord = aTexCoord;

    // 5. TBN 행렬 구축 (비균등 스케일 대응 노멀 행렬 적용)
    mat3 normalMatrix = transpose(inverse(mat3(worldMat)));
    
    vec3 T = normalize(normalMatrix * localTangent);
    vec3 N = normalize(normalMatrix * localNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);

    // 6. 최종 클립 공간 투영
    gl_Position = uScene.projection * uScene.view * worldPos;
}