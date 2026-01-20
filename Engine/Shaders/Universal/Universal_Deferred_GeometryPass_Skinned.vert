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

out vec4 vCurrClipPos; 
out vec4 vPrevClipPos;

// 카메라 UBO
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
uniform mat4 uPrevModelViewProj;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

// [안전장치 추가]
vec3 SafeNormalize(vec3 v)
{
    if (dot(v, v) < 1e-6) return vec3(0, 0, 1);
    return normalize(v);
}

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
    if (totalWeight < 1e-4 || dot(totalNormal, totalNormal) < 1e-6)
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
    vec3 T = SafeNormalize(normalMatrix * totalTangent);
    vec3 N = SafeNormalize(normalMatrix * totalNormal);
    T = SafeNormalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    if (dot(B, B) < 1e-6) 
    {
        vec3 up = abs(N.y) < 0.999 ? vec3(0, 1, 0) : vec3(0, 0, 1);
        B = normalize(cross(N, up));
    }

    Normal = N;
    TBN = mat3(T, B, N);

    // 3-1. 현재 클립 좌표
    vCurrClipPos = projection * view * worldPos;
    gl_Position = vCurrClipPos;

    // 3-2. 과거 클립 좌표
    // 주의: 애니메이션 자체의 움직임(이전 프레임의 본 위치)은 무시하고, 
    // '현재 포즈' 그대로 '카메라만 과거 위치'였을 때를 가정합니다. (Camera Motion Blur Only)
    // totalPosition은 이미 본 변형이 끝난 로컬 좌표이므로, 여기에 과거 MVP를 곱하면 됩니다.
    vPrevClipPos = uPrevModelViewProj * totalPosition;
}