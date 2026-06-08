#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uSceneTex;

// FXAA Toggle
uniform bool uEnableFXAA;

// Tone mapping
uniform int   uToneMappingMode;
uniform float uExposure;
uniform float uGamma;

// Effects
uniform float uVignetteIntensity;
uniform float uSharpenAmount;
uniform float uGrainAmount;

// CRT
uniform vec2  uDistortionK;
uniform float uScanlineIntensity;
uniform float uPhosphorIntensity;
uniform float uFlickerIntensity;
uniform float uTime;

// -------------------------------------------------------------
// FXAA (Fast Approximate Anti-Aliasing) 알고리즘
// -------------------------------------------------------------
vec3 ApplyFXAA(sampler2D tex, vec2 uv, vec2 texSize)
{
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0 / 8.0;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;

    vec2 inverseTexSize = 1.0 / texSize;
    
    vec3 rgbNW = texture(tex, uv + vec2(-1.0, -1.0) * inverseTexSize).rgb;
    vec3 rgbNE = texture(tex, uv + vec2(1.0, -1.0) * inverseTexSize).rgb;
    vec3 rgbSW = texture(tex, uv + vec2(-1.0, 1.0) * inverseTexSize).rgb;
    vec3 rgbSE = texture(tex, uv + vec2(1.0, 1.0) * inverseTexSize).rgb;
    vec3 rgbM  = texture(tex, uv).rgb;

    // 밝기(Luma) 계산
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    // 에지 방향 탐지
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * inverseTexSize;

    // 블렌딩 픽셀 샘플링
    vec3 rgbA = (1.0/2.0) * (
        texture(tex, uv + dir * (1.0/3.0 - 0.5)).rgb +
        texture(tex, uv + dir * (2.0/3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        texture(tex, uv + dir * (0.0/3.0 - 0.5)).rgb +
        texture(tex, uv + dir * (3.0/3.0 - 0.5)).rgb);

    float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB;
}

// ACES Tone Mapping Curve
vec3 ACESFilm(vec3 x) 
{
    float a = 2.51f; float b = 0.03f; float c = 2.43f; float d = 0.59f; float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// CRT Barrel Distortion
vec2 CRTDistortion(vec2 uv) 
{
    vec2 cc = uv - 0.5; float dist = dot(cc, cc);
    return uv + cc * (uDistortionK.x * dist + uDistortionK.y * dist * dist);
}

// Noise for Film Grain
float Random(vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
    vec2 uv = TexCoord;
    vec2 texSize = textureSize(uSceneTex, 0);

    // 1. CRT Screen Distortion
    if (uDistortionK.x > 0.0 || uDistortionK.y > 0.0)
    {
        uv = CRTDistortion(uv);
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
    }

    // 2. Base Color Fetch (FXAA or Sharpening)
    vec3 sceneColor = vec3(0.0);
    
    if (uSharpenAmount > 0.0)
    {
        // Sharpening
        vec2 offset = 1.0 / texSize;
        float negAmt = -uSharpenAmount;
        float centerAmt = 1.0 + 8.0 * uSharpenAmount;

        sceneColor += texture(uSceneTex, uv + vec2(-offset.x, -offset.y)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2( 0.0,      -offset.y)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2( offset.x, -offset.y)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2(-offset.x,  0.0)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv).rgb * centerAmt;
        sceneColor += texture(uSceneTex, uv + vec2( offset.x,  0.0)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2(-offset.x,  offset.y)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2( 0.0,       offset.y)).rgb * negAmt;
        sceneColor += texture(uSceneTex, uv + vec2( offset.x,  offset.y)).rgb * negAmt;
    }
    else if (uEnableFXAA)
    {
        // FXAA 적용 (Sharpening이 꺼져있을 때만 동작)
        sceneColor = ApplyFXAA(uSceneTex, uv, texSize);
    }
    else
    {
        // 기본 텍스처 샘플링
        sceneColor = texture(uSceneTex, uv).rgb;
    }

    // 3. Tone Mapping (HDR -> LDR 변환)
    if (uToneMappingMode == 0)      sceneColor = vec3(1.0) - exp(-sceneColor * uExposure);
    else if (uToneMappingMode == 1) sceneColor = ACESFilm(sceneColor * uExposure);
    else if (uToneMappingMode == 2) sceneColor = sceneColor / (sceneColor + vec3(1.0));
    else if (uToneMappingMode == 3) sceneColor = sceneColor; 

    // 4. Gamma Correction
    if (uGamma > 0.0) sceneColor = pow(sceneColor, vec3(1.0 / uGamma));

    // 5. Vignette
    if (uVignetteIntensity > 0.0)
    {
        vec2 distFromCenter = uv - 0.5;
        float vignette = 1.0 - dot(distFromCenter, distFromCenter) * uVignetteIntensity * 4.0;
        sceneColor *= clamp(vignette, 0.0, 1.0);
    }

    // 6. Film Grain
    if (uGrainAmount > 0.0)
    {
        float noise = Random(uv * (uTime + 1.0)) - 0.5;
        sceneColor += noise * uGrainAmount;
    }

    // 7. CRT Scanlines & Flicker
    if (uScanlineIntensity > 0.0)
    {
        float scanline = sin(uv.y * texSize.y * 3.141592) * uScanlineIntensity;
        sceneColor -= sceneColor * scanline;
    }

    if (uFlickerIntensity > 0.0)
    {
        float flicker = sin(uTime * 50.0) * uFlickerIntensity;
        sceneColor += flicker;
    }

    // 8. Phosphor
    if (uPhosphorIntensity > 0.0)
    {
        float offset = uPhosphorIntensity * 0.002;
        float r = texture(uSceneTex, uv + vec2(offset, 0.0)).r;
        float g = sceneColor.g;
        float b = texture(uSceneTex, uv - vec2(offset, 0.0)).b;
        sceneColor = mix(sceneColor, vec3(r, g, b), uPhosphorIntensity);
    }

    FragColor = vec4(sceneColor, 1.0);
}