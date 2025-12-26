#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

uniform float gamma;
uniform float exposure;
uniform float bloomStrength;

// 영화 같은 색감을 주는 ACES 톤 매핑 함수
vec3 ACESToneMapping(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    // 1. 텍스처 샘플링
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    // 2. Bloom 합성 (Additive Blending)
    hdrColor += bloomColor * bloomStrength;

    // 3. Tone Mapping
    // HDR(0 ~ 무한대) -> LDR(0 ~ 1) 범위로 압축
    
    // [옵션 A] 기존 방식 (Exposure Tone Mapping)
    // 장점: 밝기 조절이 직관적임 / 단점: 아주 밝은 빛이 단순히 하얗게 타버림
    // vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // [옵션 B] ACES Tone Mapping (추천)
    // 장점: 언리얼/유니티 등 상용 엔진 표준. 밝은 빛의 채도를 더 잘 유지함.
    // 사용하려면 위 코드를 주석하고 아래 주석을 푸세요.
    vec3 result = ACESToneMapping(hdrColor * exposure);
    
    // 2. Gamma Correction
    // Linear Space -> sRGB Space 변환
    result = pow(result, vec3(1.0 / gamma));

    // [디버그 모드]
    // 다 무시하고 그냥 블룸 텍스처만 출력해봅니다.
    // 만약 화면이 완전히 까맣다면 -> Bloom 생성(Phase 1)이 안 되고 있는 것
    // 만약 화면에 흐릿한 물체가 보인다면 -> Bloom은 잘 되는데 합성이 약한 것
    // FragColor = vec4(hdrColor, 1.0);
    
    // FragColor = vec4(bloomColor, 1.0);
  
    FragColor = vec4(result, 1.0);
}