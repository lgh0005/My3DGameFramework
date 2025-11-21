#version 460 core

in vec4 vertexColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex;
uniform float gamma;
uniform float exposure;

void main() 
{
	// 1. HDR 텍스쳐에서 색상 가져오기
    vec3 hdrColor = texture(tex, texCoord).rgb;

	// 2. 톤 매핑 (Tone Mapping) - Exposure 방식
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// 3. 감마 보정 (Gamma Correction)
	mapped = pow(mapped, vec3(1.0 / gamma));

	// 4. 결과
	fragColor = vec4(mapped, 1.0);
}