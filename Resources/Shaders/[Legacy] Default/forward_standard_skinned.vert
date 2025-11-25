#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIDs;  
layout (location = 5) in vec4 aWeights;    

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

out vec2 texCoords;
out vec3 normal;   
out vec3 position;
out vec4 FragPosLightSpace;
out mat3 TBN;

void main()
{
    // 0. 가중치를 합산할 최종 위치
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal   = vec3(0.0f);
    vec3 totalTangent  = vec3(0.0f);

    // 1. 4개의 뼈 영향력을 계산
    for (int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        // C++에서 -1로 초기화한 뼈는 건너뜀
        if(aBoneIDs[i] == -1)  continue;
            
        // 뼈가 이 정점에 영향을 줌
        // 1-1. 이 뼈의 최종 변환 행렬을 가져옴
        // (이 행렬은 오프셋과 애니메이션이 모두 적용된 모델 공간 기준 행렬)
        mat4 boneMatrix = finalBoneMatrices[aBoneIDs[i]];

        // 1-2. 위치, 노멀, 탄젠트 누적
        float weight = aWeights[i];
        totalPosition += (boneMatrix * vec4(aPos, 1.0f)) * weight;
        totalNormal   += (boneMatrix * vec4(aNormal, 0.0f)).xyz * weight;
        totalTangent  += (boneMatrix * vec4(aTangent, 0.0f)).xyz * weight;
    }
    
    // 2. 정적 메쉬(가중치가 0인)를 위한 처리
    // (모든 가중치가 0이면 totalPosition이 (0,0,0,0)이 되므로, 원본 위치를 사용)
    float totalWeight = aWeights[0] + aWeights[1] + aWeights[2] + aWeights[3];
    if (totalWeight == 0.0)
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal   = aNormal;
        totalTangent  = aTangent;
    }

    // 3. 모델, 뷰, 프로젝션 행렬을 적용하여 최종 위치 계산
    vec4 worldPos = model * totalPosition;
    gl_Position = projection * view * worldPos;

    // 4. TBN 행렬 구성 (World Space 기준)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * totalTangent);
    vec3 N = normalize(normalMatrix * totalNormal);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt 직교화 (T를 N에 수직하게 보정)
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    // 5. 나머지 출력 전달
    texCoords = aTexCoord;
    position = worldPos.xyz;
    normal = N; // Fallback
    FragPosLightSpace = lightSpaceMatrix * worldPos;
}