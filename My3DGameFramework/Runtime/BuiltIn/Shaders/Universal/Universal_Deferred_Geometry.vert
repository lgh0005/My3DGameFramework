#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

struct InstanceProperty {
    mat4  worldMatrix;  // 64
    vec4  color;        // 16
    uint  boneOffset;   // 4
    uint  typeFlag;     // 4 (0: Static, 1: Skinned)
    uint  pad[2];       // 8
};

layout (std430, binding = 0) readonly buffer InstanceBuffer { InstanceProperty instances[]; };
layout (std430, binding = 1) readonly buffer BoneBuffer { mat4 boneMatrices[]; };

layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 uPrevVP; // 카메라의 이전 View-Projection 행렬

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 vCurrClipPos;
out vec4 vPrevClipPos;
out vec4 vMeshColor;

const int MAX_BONE_INFLUENCE = 4;

vec3 SafeNormalize(vec3 v)
{
    return (dot(v, v) < 1e-6) ? vec3(0, 0, 1) : normalize(v);
}

void main() 
{
    InstanceProperty inst = instances[gl_InstanceID];
    
    vec4 localPos = vec4(aPos, 1.0);
    vec3 localNormal = aNormal;
    vec3 localTangent = aTangent;

    // 1. Skinned Mesh 연산 (typeFlag == 1)
    if (inst.typeFlag == 1) {
        vec4 totalPos = vec4(0.0);
        vec3 totalNormal = vec3(0.0);
        vec3 totalTangent = vec3(0.0);

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) 
        {
            int boneID = aBoneIDs[i];
            if (boneID == -1) continue;

            mat4 boneMat = boneMatrices[inst.boneOffset + boneID];
            totalPos     += (boneMat * vec4(aPos, 1.0)) * aWeights[i];
            totalNormal  += (mat3(boneMat) * aNormal) * aWeights[i];
            totalTangent += (mat3(boneMat) * aTangent) * aWeights[i];
        }
        
        if (length(totalPos) > 0.0001) 
        {
            localPos = totalPos;
            localNormal = totalNormal;
            localTangent = totalTangent;
        }
    }

    // 2. 월드 변환 및 데이터 계산
    vec4 worldPos = inst.worldMatrix * localPos;
    FragPos = worldPos.xyz;
    TexCoords = aTexCoord;
    vMeshColor = inst.color;

    // 3. Normal Matrix 및 TBN 계산 (Gram-Schmidt 포함)
    mat3 normalMatrix = transpose(inverse(mat3(inst.worldMatrix)));
    vec3 N = SafeNormalize(normalMatrix * localNormal);
    vec3 T = SafeNormalize(normalMatrix * localTangent);
    T = SafeNormalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = N;
    TBN = mat3(T, B, N);

    // 4. 모션 벡터를 위한 클립 공간 좌표
    vCurrClipPos = projection * view * worldPos;
    vPrevClipPos = uPrevVP * worldPos; // 카메라 모션 기반
    
    gl_Position = vCurrClipPos;
}