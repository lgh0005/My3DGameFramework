#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIDs;
layout (location = 5) in vec4 aWeights;

struct InstanceProperty 
{
    mat4  worldMatrix;  // 64 bytes
    vec4  color;        // 16 bytes
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
out vec4 vMeshColor;

const int MAX_BONE_INFLUENCE = 4;

void main() 
{
    InstanceProperty inst = instances[gl_InstanceID];
    
    vec4 localPos = vec4(aPos, 1.0);
    vec3 localNormal = aNormal;
    vec3 localTangent = aTangent;

    // 1. Skinned Mesh 연산 (typeFlag == 1)
    if (inst.typeFlag == 1) 
    {
        vec4 totalPosition = vec4(0.0);
        vec3 totalNormal   = vec3(0.0);
        vec3 totalTangent  = vec3(0.0);

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) 
        {
            int boneID = aBoneIDs[i];
            if (boneID == -1) continue;

            mat4 boneMat = boneMatrices[inst.boneOffset + boneID];
            totalPosition += (boneMat * vec4(aPos, 1.0)) * aWeights[i];
            totalNormal   += (mat3(boneMat) * aNormal) * aWeights[i];
            totalTangent  += (mat3(boneMat) * aTangent) * aWeights[i];
        }
        
        if (length(totalPosition) > 0.0001) 
        {
            localPos = totalPosition;
            localNormal = totalNormal;
            localTangent = totalTangent;
        }
    }

    // 2. 월드 변환
    vec4 worldPos = inst.worldMatrix * localPos;
    FragPos = worldPos.xyz;
    TexCoords = aTexCoord;
    vMeshColor = inst.color; // 인스턴스 컬러 전달

    // 3. Normal/TBN 계산
    mat3 normalMatrix = transpose(inverse(mat3(inst.worldMatrix)));
    vec3 T = normalize(normalMatrix * localTangent);
    vec3 N = normalize(normalMatrix * localNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = N;
    TBN = mat3(T, B, N);

    // 4. 모션 벡터 및 좌표 결정
    vCurrClipPos = projection * view * worldPos;
    vPrevClipPos = uPrevVP * worldPos;
    gl_Position = vCurrClipPos;
}