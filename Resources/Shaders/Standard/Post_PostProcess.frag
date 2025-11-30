#version 460 core

layout (location = 0) in vec2 texCoord;

out vec4 fragColor;

uniform float gamma;
uniform float exposure;
uniform bool bloom;
uniform sampler2D tex;
uniform sampler2D bloomBlur;
uniform vec2 inverseScreenSize;

// [추가] FXAA 알고리즘 함수
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

void main() 
{
	// 1. 원본 HDR 색상 가져오기
    // vec3 hdrColor = texture(tex, texCoord).rgb;
    vec3 hdrColor = ApplyFXAA(tex, texCoord, inverseScreenSize);

	// 2. 블러된 밝은 색상 가져오기
    vec3 bloomColor = texture(bloomBlur, texCoord).rgb;

	// 3. [핵심] 원본에 블룸 더하기 (Additive Blending)
    if (bloom)
    {
        hdrColor += bloomColor; 
    }

	// 2. 톤 매핑 (Tone Mapping) - Exposure 방식
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// 3. 감마 보정 (Gamma Correction)
	mapped = pow(mapped, vec3(1.0 / gamma));

	// 4. 결과
	fragColor = vec4(mapped, 1.0);
}