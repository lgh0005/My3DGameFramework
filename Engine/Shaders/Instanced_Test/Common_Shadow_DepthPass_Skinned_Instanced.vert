#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

// 인스턴스 및 본 데이터 구조 (G-Buffer와 일관성 유지)
struct SkinnedInstanceProperty 
{
    mat4 worldMatrix;
    uint boneOffset;
    uint pad[3];
};

layout (std430, binding = 0) readonly buffer InstanceBuffer 
{
    SkinnedInstanceProperty instances[];
};

layout (std430, binding = 1) readonly buffer BoneBuffer 
{
    mat4 globalBoneMatrices[];
};

uniform mat4 lightSpaceMatrix;

void main() 
{
    SkinnedInstanceProperty inst = instances[gl_InstanceID];
    uint offset = inst.boneOffset;
    mat4 model = inst.worldMatrix;

    vec4 totalPosition = vec4(0.0);
    for (int i = 0; i < 4; i++) 
    {
        if(aBoneIDs[i] == -1) continue;

        mat4 boneMat = globalBoneMatrices[offset + aBoneIDs[i]];
        totalPosition += (boneMat * vec4(aPos, 1.0)) * aWeights[i];
    }

    // 가중치 예외 처리
    if (totalPosition.w == 0.0) 
    {
        totalPosition = vec4(aPos, 1.0);
    }

    gl_Position = lightSpaceMatrix * model * totalPosition;
}