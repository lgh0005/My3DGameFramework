#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

struct InstanceProperty 
{
    mat4  worldMatrix;
    vec4  color;
    uint  boneOffset;
    uint  typeFlag;
    uint  pad[2];
};

layout (std430, binding = 0) readonly buffer InstanceBuffer { InstanceProperty instances[]; };
layout (std430, binding = 1) readonly buffer BoneBuffer { mat4 boneMatrices[]; };
layout (std140, binding = 0) uniform CameraData 
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

out vec4 vOutlineColor;

void main() 
{
    InstanceProperty inst = instances[gl_InstanceID];
    vec4 localPos = vec4(aPos, 1.0);

    // Skinned
    if (inst.typeFlag == 1) 
    {
        vec4 totalPos = vec4(0.0);
        for (int i = 0; i < 4; i++) 
        {
            if (aBoneIDs[i] == -1) continue;
            totalPos += (boneMatrices[inst.boneOffset + aBoneIDs[i]] * vec4(aPos, 1.0)) * aWeights[i];
        }
        if (totalPos.w > 0.0001) localPos = totalPos;
    }

    vOutlineColor = inst.color;
    gl_Position = projection * view * inst.worldMatrix * localPos;
}