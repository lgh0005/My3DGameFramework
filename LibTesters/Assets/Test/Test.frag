#version 460 core

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vWorldPos;
layout(location = 2) in vec3 vNormal;

layout(location = 0) out vec4 FragColor;

// [UBO] C++ MaterialData와 완벽하게 일치 (std140)
layout(std140, binding = 2) uniform MaterialBlock 
{
    vec4  albedoFactor;      // 0  (16 bytes)
    vec3  emissiveFactor;    // 16 (12 bytes)
    float emissionStrength;  // 28 (4 bytes) 
    float shininess;         // 32 (4 bytes)
    float heightScale;       // 36 (4 bytes)
    float metallicFactor;    // 40 (4 bytes)
    float roughnessFactor;   // 44 (4 bytes)
};

// [Sampler] 머티리얼 텍스쳐
layout(binding = 0) uniform sampler2D uAlbedoMap;

void main() 
{
    vec4 texColor = texture(uAlbedoMap, vTexCoord);
    
    // 테스트 2: 라이팅, 머티리얼 팩터 다 무시하고 텍스처 원본만 출력
    FragColor = vec4(texColor.rgb, 1.0);

    // 1. 텍스처 샘플링 (이미지가 없으면 Unbind 로직에 의해 검은색(0)으로 나옴)
    //vec4 texColor = texture(uAlbedoMap, vTexCoord);

    // 2. 머티리얼 팩터 적용
    // 텍스처가 없는 경우를 대비해 나중에는 텍스처 존재 여부 플래그를 쓸 수도 있지만,
    // 지금은 단순 샘플링 결과와 팩터를 곱합니다.
    //vec3 albedo = texColor.rgb * albedoFactor.rgb;

    // 3. (옵션) 아주 기본적인 라이팅 맛보기 (방향광 가정)
    //vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    //vec3 N = normalize(vNormal);
    //float diff = max(dot(N, L), 0.2);

    // 최종 색상 결정
    //FragColor = vec4(albedo * diff, albedoFactor.a);
}