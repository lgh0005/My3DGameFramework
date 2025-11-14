#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;  
layout (location = 4) in vec4 aWeights;    

// 2. 유니폼
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// 3. 뼈 행렬 배열 (Animator가 계산)
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4; // Vertex.h와 일치
uniform mat4 finalBoneMatrices[MAX_BONES];

// 4. 프래그먼트 셰이더로 전달
out vec2 TexCoords;
out vec3 normal;   
out vec3 position;

void main()
{
    // 0. 가중치를 합산할 최종 위치
    vec4 totalPosition = vec4(0.0f);
    vec4 totalNormal = vec4(0.0f);

    // 1. 4개의 뼈 영향력을 계산
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        // C++에서 -1로 초기화한 뼈는 건너뜀
        if(aBoneIDs[i] == -1) 
            continue;
            
        // 뼈가 이 정점에 영향을 줌
        
        // 1-1. 이 뼈의 최종 변환 행렬을 가져옴
        // (이 행렬은 오프셋과 애니메이션이 모두 적용된 모델 공간 기준 행렬)
        mat4 boneMatrix = finalBoneMatrices[aBoneIDs[i]];

        // 1-2. 원본 위치에 뼈 변환을 적용하고, 가중치(weight)를 곱해서 누적
        totalPosition += (boneMatrix * vec4(aPos, 1.0f)) * aWeights[i];
        totalNormal += (boneMatrix * vec4(aNormal, 0.0f)) * aWeights[i];
    }
    
    // 2. 정적 메쉬(가중치가 0인)를 위한 처리
    // (모든 가중치가 0이면 totalPosition이 (0,0,0,0)이 되므로, 원본 위치를 사용)
    float totalWeight = aWeights[0] + aWeights[1] + aWeights[2] + aWeights[3];
    if (totalWeight == 0.0)
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = vec4(aNormal, 0.0f);
    }

    // 3. 모델, 뷰, 프로젝션 행렬을 적용하여 최종 위치 계산
    gl_Position = projection * view * model * totalPosition;

    // 4. 텍스처 좌표 전달
    TexCoords = aTexCoord;
    position = (model * totalPosition).xyz;
    normal = normalize((transpose(inverse(model)) * totalNormal).xyz);
}