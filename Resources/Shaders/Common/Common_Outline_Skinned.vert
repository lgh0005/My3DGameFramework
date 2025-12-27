#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in ivec4 aBoneIDs; // Bone ID
layout (location = 5) in vec4 aWeights;  // Bone Weight

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
uniform float outlineThickness; // 0.0이면 원본, >0.0이면 확장

// 애니메이션 관련
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal   = vec3(0.0);

    // 조건문 없이 바로 스키닝 연산 수행
    bool hasBone = false;
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIDs[i] == -1) continue;
        if(aBoneIDs[i] >= MAX_BONES) break;
        
        hasBone = true;

        // 위치 누적
        vec4 localPosition = finalBoneMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];

        // 노멀 누적 (회전만 고려)
        vec3 localNormal = mat3(finalBoneMatrices[aBoneIDs[i]]) * aNormal;
        totalNormal += localNormal * aWeights[i];
    }
    
    // 안전장치
    if (!hasBone) 
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = aNormal;
    }

    vec4 worldPos = model * totalPosition;
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * totalNormal);

    worldPos.xyz += worldNormal * outlineThickness;
    gl_Position = projection * view * worldPos;
}