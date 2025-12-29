#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in ivec4 aBoneIDs; 
layout (location = 5) in vec4 aWeights;  

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);

    bool hasBone = false;
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIDs[i] == -1) continue;
        if(aBoneIDs[i] >= MAX_BONES) break;
        
        hasBone = true;
        vec4 localPosition = finalBoneMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
    }
    
    if (!hasBone) 
    {
        totalPosition = vec4(aPos, 1.0f);
    }

    gl_Position = projection * view * model * totalPosition;
}