#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

// [Input Textures]
uniform sampler2D uSceneTex;       // 0: 이전 패스의 HDR 결과물
uniform sampler2D uPositionTex;    // 1: G-Buffer Position (.xyz: World Pos, .w: AO)

// [Parameters]
uniform float uPixelSize;      
uniform int   uDilationRadius; 
uniform float uPostLevels;     

uniform vec3  uFogColor;
uniform float uFogDensity;
uniform vec3  uCameraPos;

void main()
{
    vec2 uv = TexCoord;
    vec2 texSize = vec2(textureSize(uSceneTex, 0));

    // ---------------------------------------------------------
    // 1. Pixelization: UV를 먼저 뭉개서 이후 모든 샘플링을 칸 단위로 강제
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
    // 4. Fog (거리 기반 지수 안개)
    // ---------------------------------------------------------
    vec3 worldPos = texture(uPositionTex, uv).xyz;
    
    // 월드 위치가 원점(0,0,0)에 근접해 있다면 하늘(Skybox)일 확률이 높음.
    // G-Buffer 렌더링 시 배경 부분의 Position이 0이라면 안개 거리 계산에서 예외 처리할 수 있습니다.
    // (완벽한 처리를 위해서는 Depth 텍스처를 활용하는 것도 방법입니다)
    float dist = length(worldPos - uCameraPos);
    
    // 0보다 클 때만 안개 연산 (밀도가 0이면 안개 효과 무시)
    if (uFogDensity > 0.0) 
    {
        float fogFactor = exp(-uFogDensity * dist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        sceneColor = mix(uFogColor, sceneColor, fogFactor);
    }

    FragColor = vec4(sceneColor, 1.0);
}