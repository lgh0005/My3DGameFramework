#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

// [Input Textures]
uniform sampler2D uColorTexture;    // 원본 화면 (Deferred Lighting 결과, Texture Unit 0)
uniform sampler2D uVelocityTexture; // 속도 맵 (G-Buffer Attachment 4, Texture Unit 1)

// [Parameters]
uniform float uBlurScale = 1.0;     // 블러 강도 조절 (기본 1.0)
uniform int uSamples = 16;           // 샘플링 횟수 (높을수록 부드럽지만 성능 무거움)

void main()
{
    // 1. 속도 벡터 읽기 (RG 채널)
    // Geometry Pass에서 (CurrNDC - PrevNDC) * 0.5 로 저장했으므로,
    // 이 값은 이미 Screen Space(UV) 기준의 이동량입니다.
    vec2 velocity = texture(uVelocityTexture, TexCoords).xy;

    // [최적화] 속도가 거의 0이면(정지 상태), 비싼 샘플링 루프 돌지 말고 원본 리턴
    float speed = length(velocity);
    if (speed < 1e-5) // 0.00001 미만이면 정지로 간주
    {
        FragColor = texture(uColorTexture, TexCoords);
        return;
    }

    // 2. 블러 강도 적용
    // (속도가 너무 빠를 때 화면이 찢어지는 걸 막기 위해 clamp를 걸 수도 있습니다)
    // velocity = clamp(velocity, -0.05, 0.05); 
    velocity *= uBlurScale;

    // 3. 모션 블러 샘플링 (Reconstruction Filter)
    vec3 color = texture(uColorTexture, TexCoords).rgb;
    
    // 현재 픽셀(t)에서 과거(t-1) 방향으로 거슬러 올라가며 샘플링합니다.
    // Velocity = (Current - Prev) 이므로, Past = Current - Velocity 입니다.
    // 따라서 텍스처 좌표에서 offset을 '빼는' 방향으로 이동합니다.
    for(int i = 1; i < uSamples; ++i)
    {
        // 0 ~ 1 사이 비율 (현재 픽셀에 가까울수록 가중치 높게 줄 수도 있음)
        float t = float(i) / float(uSamples - 1);
        
        // 과거 지점 계산
        vec2 offset = velocity * t;
        vec2 sampleCoords = TexCoords - offset;

        // [안전장치] 화면 밖을 참조하지 않도록 UV 범위 체크 (선택 사항)
        // if(sampleCoords.x < 0.0 || sampleCoords.x > 1.0 || sampleCoords.y < 0.0 || sampleCoords.y > 1.0) continue;

        // 색상 누적
        color += texture(uColorTexture, sampleCoords).rgb;
    }

    // 4. 평균 내기
    FragColor = vec4(color / float(uSamples), 1.0);
}