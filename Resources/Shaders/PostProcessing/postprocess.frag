#version 460 core

layout (location = 0) in vec2 texCoord;

out vec4 fragColor;

uniform float gamma;
uniform float exposure;
uniform bool bloom;
uniform sampler2D tex;
uniform sampler2D bloomBlur;

void main() 
{
	// 1. 원본 HDR 색상 가져오기
    vec3 hdrColor = texture(tex, texCoord).rgb;

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