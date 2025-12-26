#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture; // 원본 HDR
uniform sampler2D bloomTexture;  // Bloom 결과

uniform float gamma;
uniform float exposure;
uniform float bloomStrength;
uniform vec2 inverseScreenSize; // (1.0/width, 1.0/height)
uniform bool useFXAA;           // FXAA On/Off

// =========================================================
// ACES Tone Mapping
// =========================================================
vec3 ACESToneMapping(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// =========================================================
// FXAA Algorithm
// =========================================================
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

    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB;
}

// =========================================================
// Main
// =========================================================
void main() 
{
    vec3 hdrColor;

    // 1. 원본 HDR 색상 가져오기 (FXAA 적용 여부 결정)
    if (useFXAA)
    {
        // 원본 텍스처에 FXAA를 적용해서 가져옴
        hdrColor = ApplyFXAA(screenTexture, TexCoords, inverseScreenSize);
    }
    else
    {
        // 그냥 가져옴
        hdrColor = texture(screenTexture, TexCoords).rgb;
    }

    // 2. 블룸(Blur) 텍스처 가져오기
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    // 3. 합성 (Additive Blending)
    // 안티 앨리어싱된 원본 위에 빛을 더함
    hdrColor += bloomColor * bloomStrength; 

    // 4. 톤 매핑 (ACES)
    vec3 mapped = ACESToneMapping(hdrColor * exposure);

    // 5. 감마 보정
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}