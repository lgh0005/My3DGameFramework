#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 vTexCoord;

// C++의 SkyboxData 구조체와 완벽하게 일치합니다. (바인딩 인덱스 2 사용)
layout(std140, binding = 2) uniform SkyboxBuffer 
{
    mat4 uView;
    mat4 uProjection;
    vec4 uTint;
    float uIntensity;
    float uRotation;
};

void main()
{
    // 1. Y축 기준 회전 적용 (스카이박스 자체를 회전)
    float s = sin(uRotation);
    float c = cos(uRotation);
    mat3 rotY = mat3
    (
        c, 0.0, -s,
        0.0, 1.0, 0.0,
        s, 0.0, c
    );
    
    // 텍스처 샘플링을 위한 3D 방향 벡터
    vTexCoord = rotY * aPos; 
    
    // 2. 뷰-투영 변환 적용
    vec4 pos = uProjection * uView * vec4(aPos, 1.0);
    
    // 3. Z를 W로 설정 (원근 분할 후 깊이값이 w/w = 1.0이 되어 항상 가장 먼 곳에 그려짐)
    gl_Position = pos.xyww;
}