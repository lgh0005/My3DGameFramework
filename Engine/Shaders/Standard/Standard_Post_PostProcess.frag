#version 460 core

layout (location = 0) in vec2 texCoord;
out vec4 fragColor;

// [1. 기본 설정 및 화면 정보]
uniform float time;              // 그레인/플리커 애니메이션용
uniform float gamma;
uniform float exposure;
uniform vec2 inverseScreenSize;  // 1.0 / 해상도

// [2. 텍스처 샘플러]
uniform sampler2D tex;           // [Slot 0] HDR Scene
uniform sampler2D bloomBlur;     // [Slot 1] Bloom
uniform sampler2D cameraDirtTex; // [Slot 2] Dirt

// [3. 블룸 및 먼지(Dirt) 설정]
uniform bool bloom;
uniform float bloomStrength;
uniform float dirtIntensity;     
uniform float dirtAmbient;

// [4. CRT 및 레트로 효과 설정]
uniform vec2  distortionK;        // CRT 곡률 (배럴 왜곡)
uniform float scanlineIntensity;  // 가로 주사선 강도
uniform float phosphorIntensity;  // 세로 격자(Phosphor) 강도
uniform float flickerIntensity;   // 화면 미세 떨림

// [5. 렌즈 및 후처리 효과 설정]
uniform float chromaticAmount;    // 색수차 강도
uniform float vignetteIntensity;  // 비네팅 강도
uniform float grainAmount;        // 필름 그레인 강도
uniform float sharpenAmount;      // 선명도 강도
uniform int   toneMappingMode;    // 0: Exposure, 1: ACES, 2: Reinhard

// [Helper Functions: 필터링 및 왜곡, 랜덤 노이즈]
vec2 Curvature(vec2 uv) 
{
    vec2 cc = uv - 0.5;
    float r2 = dot(cc, cc);
    return (cc * (1.0 + distortionK.x * r2 + distortionK.y * r2 * r2)) + 0.5;
}

float hash(float n) { return fract(sin(n) * 43758.5453123); } 

// 1. ACES Filmic Tone Mapping (Narkowicz 2015)
vec3 ACESToneMapping(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// 2. Exposure Tone Mapping (기존 방식 함수화)
vec3 ExposureToneMapping(vec3 color, float exposure)
{
    return vec3(1.0) - exp(-color * exposure);
}

// 3. Reinhard Tone Mapping (차분함, 하이라이트 보존)
vec3 ReinhardToneMapping(vec3 x)
{
    return x / (x + vec3(1.0));
}

// FXAA 알고리즘 함수
vec3 ApplyFXAA(sampler2D tex, vec2 coords, vec2 inverseScreenSize) 
{
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0/8.0;
    float FXAA_REDUCE_MIN = 1.0/128.0;

    vec3 rgbNW = texture(tex, coords + (vec2(-1.0, -1.0) * inverseScreenSize)).xyz;
    vec3 rgbNE = texture(tex, coords + (vec2(1.0, -1.0) * inverseScreenSize)).xyz;
    vec3 rgbSW = texture(tex, coords + (vec2(-1.0, 1.0) * inverseScreenSize)).xyz;
    vec3 rgbSE = texture(tex, coords + (vec2(1.0, 1.0) * inverseScreenSize)).xyz;
    vec3 rgbM  = texture(tex, coords).xyz;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
          dir * rcpDirMin)) * inverseScreenSize;

    vec3 rgbA = (1.0/2.0) * (
        texture(tex, coords.xy + dir * (1.0/3.0 - 0.5)).xyz +
        texture(tex, coords.xy + dir * (2.0/3.0 - 0.5)).xyz);

    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        texture(tex, coords.xy + dir * (0.0/3.0 - 0.5)).xyz +
        texture(tex, coords.xy + dir * (3.0/3.0 - 0.5)).xyz);

    float lumaB = dot(rgbB, luma);

    if((lumaB < lumaMin) || (lumaB > lumaMax))
    {
        return rgbA;
    } 
    else 
    {
        return rgbB;
    }
}

// 3. Sharpen
vec3 ApplySharpen(sampler2D t, vec2 uv, float amt) 
{
    if (amt <= 0.0) return texture(t, uv).rgb;

    vec3 center = ApplyFXAA(t, uv, inverseScreenSize);
    vec2 offset = inverseScreenSize * 1.5; 
    
    vec3 neighbor = (
        texture(t, uv + vec2(offset.x, 0)).rgb +
        texture(t, uv - vec2(offset.x, 0)).rgb +
        texture(t, uv + vec2(0, offset.y)).rgb +
        texture(t, uv - vec2(0, offset.y)).rgb
    ) * 0.25;
    return mix(center, center + (center - neighbor), amt);
}

void main() 
{
    // [단계 1] 기하학 왜곡 (CRT Curvature)
    vec2 uv = Curvature(texCoord);

    // 화면 밖 영역 검은색 처리
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) 
    {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // [단계 2] HDR 샘플링 및 색수차 (Chromatic Aberration);
    float dist = distance(uv, vec2(0.5));
    float chromOffset = dist * chromaticAmount;
    
    vec3 hdrColor;
    hdrColor.r = ApplySharpen(tex, uv + vec2(chromOffset, 0.0), sharpenAmount).r;
    hdrColor.g = ApplySharpen(tex, uv, sharpenAmount).g;
    hdrColor.b = ApplySharpen(tex, uv - vec2(chromOffset, 0.0), sharpenAmount).b;

    // [단계 3] 블룸 및 렌즈 먼지 합성
    if (bloom) 
    {
        vec3 bloomColor = texture(bloomBlur, uv).rgb;
        vec3 dirtData = texture(cameraDirtTex, uv).rgb;
        vec3 finalBloom = bloomColor * bloomStrength;
        hdrColor += finalBloom + (finalBloom * dirtData * dirtIntensity) + (dirtData * dirtAmbient);
    }

    // [단계 4] 톤 매핑 (HDR -> LDR)
    vec3 mapped;
    switch (toneMappingMode) 
    {
        case 0: mapped = ExposureToneMapping(hdrColor, exposure); break;
        case 1: mapped = ACESToneMapping(hdrColor * exposure); break;
        case 2: mapped = ReinhardToneMapping(hdrColor * exposure); break;
        default: mapped = ACESToneMapping(hdrColor * exposure); break;
    }

    // [단계 6] 스타일링 (LDR 영역)
    
    // 6-1. 비네팅
    mapped *= smoothstep(0.8, 0.5 - vignetteIntensity, dist);

    // 6-2. 필름 그레인
    float noise = (fract(sin(dot(uv, vec2(12.9898, 78.233) * (time * 0.1))) * 43758.5453));
    mapped += (noise - 0.5) * grainAmount;

    // 6-3. CRT 레트로 효과 (스캔라인 + 격자 + 플리커)
    // 6-3-1. 가로 스캔라인 (정적)
    float scanline = sin(uv.y * (1.0 / inverseScreenSize.y) * 2.0) * 0.5 + 0.5;
    mapped *= mix(1.0, scanline, scanlineIntensity);

    // 6-3-2. 세로 RGB 격자 (정적)
    float phosphor = sin(uv.x * (1.0 / inverseScreenSize.x) * 3.0) * 0.2 + 0.8;
    mapped *= mix(1.0, phosphor, phosphorIntensity);

    // 6-3-3. 움직이는 노이즈 띠 (Rolling Bar)
    float rollingBar = sin(uv.y * 2.0 + time * 1.5 + hash(time) * 0.1);
    rollingBar = smoothstep(0.95, 1.0, rollingBar);

    // 6-3-4. 전압 강하 플리커 (Voltage Drop Flicker)
    float baseFlicker = sin(time * 2.0) * 0.5 + 0.5; // 천천히 울렁거림
    float dropTrigger = hash(time * 15.0);           // 불규칙한 트리거
    float voltageDrop = (dropTrigger > 0.995) ? 0.85 : 1.0; // 가끔 틱 하고 어두워짐
    float finalFlicker = (1.0 + (baseFlicker - 0.5) * flickerIntensity) * voltageDrop;
    mapped *= finalFlicker;
    mapped += rollingBar * flickerIntensity * 2.0;

    // [단계 7] 감마 보정 및 최종 출력
    mapped = pow(mapped, vec3(1.0 / gamma));
    fragColor = vec4(mapped, 1.0);
}