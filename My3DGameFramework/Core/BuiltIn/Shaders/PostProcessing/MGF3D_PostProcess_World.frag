#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

// [Input Textures]
uniform sampler2D uSceneTex;       
uniform sampler2D uPositionTex;    

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
    // 1. Pixelization (텍셀 중앙 샘플링 보정 포함)
    // ---------------------------------------------------------
    if (uPixelSize > 1.0)
    {
        vec2 d = uPixelSize / texSize;
        uv = d * (floor(uv / d) + 0.5);
    }

    // ---------------------------------------------------------
    // 2. Dilation (가장 밝은 픽셀 팽창)
    // ---------------------------------------------------------
    vec3 sceneColor = vec3(0.0);
    if (uDilationRadius > 0)
    {
        vec3 maxC = vec3(0.0);
        for (int x = -uDilationRadius; x <= uDilationRadius; ++x)
        {
            for (int y = -uDilationRadius; y <= uDilationRadius; ++y)
            {
                vec2 o = vec2(x, y) / texSize;
                // LOD 0번 강제 샘플링 (미분 아티팩트 방지)
                maxC = max(maxC, textureLod(uSceneTex, uv + o, 0.0).rgb);
            }
        }
        sceneColor = maxC;
    }
    else
    {
        sceneColor = textureLod(uSceneTex, uv, 0.0).rgb;
    }

    // ---------------------------------------------------------
    // 3. Posterization (색상 단계 제한)
    // ---------------------------------------------------------
    if (uPostLevels > 0.0)
    {
        sceneColor = floor(sceneColor * uPostLevels) / uPostLevels;
    }

    // ---------------------------------------------------------
    // 4. Fog (거리 기반 지수 안개)
    // ---------------------------------------------------------
    if (uFogDensity > 0.0) 
    {
        vec3 worldPos = textureLod(uPositionTex, uv, 0.0).xyz;
        float dist = length(worldPos - uCameraPos);
        
        float fogFactor = exp(-uFogDensity * dist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        sceneColor = mix(uFogColor, sceneColor, fogFactor);
    }

    FragColor = vec4(sceneColor, 1.0);
}