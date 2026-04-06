#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

// [Input Textures]
uniform sampler2D uSceneTex;       // 이전 패스의 HDR 결과물
uniform sampler2D uDepthTex; // context->GetTexture(RenderSlot::GVelocity)

// [Parameters]
uniform float uPixelSize;      // 1.0보다 크면 활성화
uniform int   uDilationRadius; // 0보다 크면 활성화
uniform float uPostLevels;     // 0.0보다 크면 활성화
uniform vec3  uFogColor;
uniform float uFogDensity;

void main()
{
    vec2 uv = TexCoord;
    vec2 texSize = textureSize(uSceneTex, 0);

    // ---------------------------------------------------------
    // 1. Pixelization: UV를 먼저 뭉개서 이후 모든 샘플링을 '칸' 단위로 강제
    // ---------------------------------------------------------
    if (uPixelSize > 1.0)
    {
        vec2 d = uPixelSize / texSize;
        uv = d * floor(uv / d);
    }

    // ---------------------------------------------------------
    // 2. Dilation: 주변 픽셀 중 가장 밝은 것을 선택하여 실루엣 확장
    // ---------------------------------------------------------
    vec3 sceneColor;
    if (uDilationRadius > 0)
    {
        vec3 maxC = vec3(0.0);
        // 주의: 반경이 커지면 성능이 급격히 저하되므로 C++에서 제한 필요
        for (int x = -uDilationRadius; x <= uDilationRadius; ++x)
        {
            for (int y = -uDilationRadius; y <= uDilationRadius; ++y)
            {
                vec2 offset = vec2(x, y) / texSize;
                maxC = max(maxC, texture(uSceneTex, uv + offset).rgb);
            }
        }
        sceneColor = maxC;
    }
    else
    {
        sceneColor = texture(uSceneTex, uv).rgb;
    }

    // ---------------------------------------------------------
    // 3. Posterization: 색상의 단계를 강제로 제한 (Retro/Cartoon Look)
    // ---------------------------------------------------------
    if (uPostLevels > 0.0)
    {
        sceneColor = floor(sceneColor * uPostLevels) / uPostLevels;
    }

    // ---------------------------------------------------------
    // 4. Fog: GVelocity.z에 구워진 선형 깊이 데이터를 바로 사용
    // ---------------------------------------------------------
    // 별도의 선형화 공식(uNear, uFar) 없이 바로 월드 거리를 읽어옵니다.
    float linearDist = texture(uDepthTex, uv).z;
    
    // 지수 안개 공식: f = e^(-density * distance)
    float fogFactor = exp(-uFogDensity * linearDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // 안개 색상과 장면 색상 혼합
    vec3 finalColor = mix(uFogColor, sceneColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}