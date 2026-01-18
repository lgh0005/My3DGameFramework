#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

out vec4 vCurrClipPos; // 현재 프레임의 클립 공간 위치
out vec4 vPrevClipPos; // 이전 프레임의 클립 공간 위치

// 카메라 UBO
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
uniform mat4 uPrevModelViewProj;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoord;
    
    // Normal Matrix 계산 (Model 행렬의 역전치 행렬)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    
    // Gram-Schmidt 직교화 (T를 N에 수직하게 재보정)
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    Normal = N; // TBN이 필요 없는 경우를 대비한 백업
    TBN = mat3(T, B, N);
    
    // 1. 현재 클립 위치 (OpenGL 기본)
    vCurrClipPos = projection * view * worldPos;
    gl_Position = vCurrClipPos;

    // 2. ★ 과거 클립 위치 계산
    // 셰이더는 단순합니다. 그냥 행렬 곱해주면 끝.
    vPrevClipPos = uPrevModelViewProj * vec4(aPos, 1.0);
}