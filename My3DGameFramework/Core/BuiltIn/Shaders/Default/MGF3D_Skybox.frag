#version 460 core
out vec4 FragColor;

in vec3 vTexCoord;

// 바인딩 슬롯 0번에는 EnvironmentMap의 m_skybox 텍스처가 들어옵니다.
layout(binding = 0) uniform samplerCube uSkybox;

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
    // 1. 큐브맵에서 HDR 환경 색상 샘플링
    vec3 envColor = texture(uSkybox, vTexCoord).rgb;
    
    // 2. 틴트(색상 필터)와 밝기 적용
    envColor = envColor * uTint.rgb * uIntensity;
    
    // (선택 사항) 만약 여기서 바로 모니터 화면으로 나간다면 톤매핑(Tone Mapping)과 
    // 감마 교정(Gamma Correction)이 필요할 수 있습니다. 
    // 하지만 보통 포스트 프로세싱 패스에서 일괄 처리하므로 여기서는 선형(Linear) HDR 값을 그대로 내보냅니다.
    FragColor = vec4(envColor, 1.0);
}