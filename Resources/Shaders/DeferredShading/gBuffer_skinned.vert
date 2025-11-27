#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

// TEMP : UBO 테스트 중
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal   = vec3(0.0f);
    vec3 totalTangent  = vec3(0.0f);
    
    // 1. 본 가중치 계산
    for (int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIDs[i] == -1)  continue;
        if(aBoneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        
        vec4 localPosition = finalBoneMatrices[aBoneIDs[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * aWeights[i];
        
        vec3 localNormal = mat3(finalBoneMatrices[aBoneIDs[i]]) * aNormal;
        totalNormal += localNormal * aWeights[i];
        
        vec3 localTangent = mat3(finalBoneMatrices[aBoneIDs[i]]) * aTangent;
        totalTangent += localTangent * aWeights[i];
    }
    
    // 가중치가 없는 경우 (혹은 오류) 원본 사용
    float totalWeight = aWeights[0] + aWeights[1] + aWeights[2] + aWeights[3];
    if (totalWeight == 0.0)
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal   = aNormal;
        totalTangent  = aTangent;
    }

    // 2. 월드 변환
    vec4 worldPos = model * totalPosition;
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * totalTangent);
    vec3 N = normalize(normalMatrix * totalNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = N;
    TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}