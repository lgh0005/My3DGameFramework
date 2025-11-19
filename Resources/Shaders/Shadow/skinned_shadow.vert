#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in ivec4 aBoneIds; 
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 100; // C++ 코드와 맞춰야 함
const int MAX_BONE_INFLUENCE = 4;

// 유니폼 변수
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 finalBoneMatrices[MAX_BONES]; // 애니메이션 행렬 배열

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        // 유효하지 않은 뼈대 ID(-1)인 경우 건너뜀
        if(aBoneIds[i] == -1) 
            continue;
            
        if(aBoneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }

        // (해당 뼈의 변환 행렬 * 로컬 위치) * 가중치
        vec4 localPosition = finalBoneMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
    }
    
    // 만약 가중치 합이 0이라면(혹은 예외 상황), 기본 위치 사용
    if (totalPosition.w == 0.0f) 
    {
        totalPosition = vec4(aPos, 1.0f);
    }

    // 최종 변환: 스킨된 로컬 위치 -> 월드 -> 빛 공간
    gl_Position = lightSpaceMatrix * model * totalPosition;
}