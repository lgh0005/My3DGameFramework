#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

struct InstanceProperty 
{
    mat4  worldMatrix;  // 64 bytes
    vec4  color;        // 16 bytes (Shadow Pass에선 무시 가능)
    uint  boneOffset;   // 4 bytes
    uint  typeFlag;     // 4 bytes (0: Static, 1: Skinned)
    uint  pad[2];       // 8 bytes
};

layout (std430, binding = 0) readonly buffer InstanceBuffer 
{
    InstanceProperty instances[];
};

layout (std430, binding = 1) readonly buffer BoneBuffer 
{
    mat4 boneMatrices[];
};

uniform mat4 uLightSpaceMatrix;

const int MAX_BONE_INFLUENCE = 4;

void main() 
{
    InstanceProperty inst = instances[gl_InstanceID];
    
    vec4 localPos = vec4(aPos, 1.0);

    // 1. Skinned Mesh 연산 (typeFlag == 1)
    if (inst.typeFlag == 1) 
    {
        vec4 totalPosition = vec4(0.0);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) 
        {
            int boneID = aBoneIDs[i];
            if (boneID == -1) continue;

            mat4 boneMat = boneMatrices[inst.boneOffset + boneID];
            totalPosition += (boneMat * vec4(aPos, 1.0)) * aWeights[i];
        }
        
        if (totalPosition.w > 0.0001) 
        {
            localPos = totalPosition;
        }
    }

    // 2. 조명 공간으로 변환: LightVP * World * LocalPos
    gl_Position = uLightSpaceMatrix * inst.worldMatrix * localPos;
}