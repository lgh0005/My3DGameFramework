#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

struct SkinnedInstanceProperty 
{
    mat4 worldMatrix;    // 64 bytes
    uint boneOffset;     // 4 bytes
    uint pad[3];         // 12 bytes (정렬용)
};

layout (std430, binding = 0) readonly buffer InstanceBuffer 
{
    SkinnedInstanceProperty instances[];
};

layout (std430, binding = 1) readonly buffer BoneBuffer 
{
    mat4 globalBoneMatrices[];
};

layout (std140, binding = 0) uniform CameraData 
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 uPrevVP;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 vCurrClipPos;
out vec4 vPrevClipPos;

const int MAX_BONE_INFLUENCE = 4;

void main() 
{
    SkinnedInstanceProperty inst = instances[gl_InstanceID];
    uint offset = inst.boneOffset;
    mat4 model = inst.worldMatrix;

    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal   = vec3(0.0);
    vec3 totalTangent  = vec3(0.0);

    // 본 가중치 연산
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) 
    {
        int boneID = aBoneIDs[i];
        if (boneID == -1) continue;

        // 전역 본 버퍼에서 자신의 offset을 더해 참조
        mat4 boneMat = globalBoneMatrices[offset + boneID];

        totalPosition += (boneMat * vec4(aPos, 1.0)) * aWeights[i];
        totalNormal   += (mat3(boneMat) * aNormal) * aWeights[i];
        totalTangent  += (mat3(boneMat) * aTangent) * aWeights[i];
    }

    // 가중치가 아예 없는 예외 처리
    if (length(totalPosition) < 0.0001) 
    {
        totalPosition = vec4(aPos, 1.0);
        totalNormal   = aNormal;
        totalTangent  = aTangent;
    }

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

    vCurrClipPos = projection * view * worldPos;
    gl_Position = vCurrClipPos;

    // 과거 클립 공간 위치 (카메라 모션 블러 전용)
    vPrevClipPos = uPrevVP * worldPos;
}