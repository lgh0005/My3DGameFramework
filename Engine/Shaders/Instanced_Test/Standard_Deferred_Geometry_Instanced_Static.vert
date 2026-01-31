#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

struct StaticInstanceProperty { mat4 worldMatrix; };

layout (std430, binding = 0) readonly buffer InstanceBuffer 
{
    StaticInstanceProperty instances[];
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

void main() 
{
    // gl_InstanceID를 인덱스로 사용하여 자신의 월드 행렬 획득
    mat4 model = instances[gl_InstanceID].worldMatrix;

    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoord;

    // Normal Matrix 계산
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = N;
    TBN = mat3(T, B, N);

    // 현재 클립 공간 위치
    vCurrClipPos = projection * view * worldPos;
    gl_Position = vCurrClipPos;

    // 과거 클립 공간 위치 (카메라 모션 블러 전용)
    // 현재 월드 위치에 과거의 카메라 행렬만 적용합니다.
    vPrevClipPos = uPrevVP * worldPos;
}