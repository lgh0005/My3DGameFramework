#version 460 core
layout(location = 0) in vec3 aPos;

// 1. 카메라 뷰-프로젝션 행렬 (임시로 Uniform 사용, 나중에 UBO Slot 0으로 전환 추천)
uniform mat4 uViewProj;

// 2. C++에서 보낸 SSBO (Slot 1) 구조체 정의
struct InstanceData
{
    mat4 worldMatrix;
};

// std430 레이아웃으로 바인딩 슬롯 1번과 연결
layout(std430, binding = 1) readonly buffer InstanceBuffer 
{
    InstanceData instances[];
};

void main()
{
    // 테스트 A: 월드 행렬만 적용 (ViewProj 무시)
    // 결과가 빨간 사각형이라면? -> SSBO 데이터는 정상입니다.
    gl_Position = uViewProj * instances[gl_InstanceID].worldMatrix * vec4(aPos, 1.0);

    // 테스트 B: 유니폼만 적용 (World 무시)
    // 결과가 빨간 사각형이라면? -> 유니폼 데이터는 정상입니다.
    // gl_Position = uViewProj * vec4(aPos, 1.0);

    // 테스트 C: 둘 다 적용하되 World를 단위 행렬로 가정
    // gl_Position = uViewProj * mat4(1.0) * vec4(aPos, 1.0);
 }