#version 460 core

// 메쉬의 기본 정점 데이터
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in ivec4 aBoneIDs; 
layout(location = 5) in vec4 aWeights;

struct SkinnedInstanceData
{
    mat4 worldMatrix;
    uint boneOffset;
    uint pad1;
    uint pad2;
    uint pad3;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer { SkinnedInstanceData instances[]; };
layout(std430, binding = 9) readonly buffer BoneBuffer { mat4 uBones[]; };

void main() 
{
    mat4 worldMat = instances[gl_InstanceID].worldMatrix;
    uint boneOffset = instances[gl_InstanceID].boneOffset;

    // 1. Bone 애니메이션 행렬 계산 (인스턴스 Offset 적용)
    mat4 boneTransform = mat4(0.0);
    for (int i = 0; i < 4; ++i)
    {
        if(aBoneIDs[i] >= 0)
        {
            uint finalBoneIndex = boneOffset + uint(aBoneIDs[i]);
            boneTransform += uBones[finalBoneIndex] * aWeights[i];
        }
    }

    // 2. 로컬 애니메이션 정점을 월드 좌표로 변환
    vec4 localPos = boneTransform * vec4(aPos, 1.0);
    vec4 worldPos = worldMat * localPos;

    // 3. 지오메트리 셰이더로 넘김 (CSM은 여기서 멈춤)
    gl_Position = worldPos;
}