#version 460 core

out vec4 fragColor;
in vec2 TexCoord;

uniform sampler2D uSceneTex;
uniform sampler2D uDepthTex; // .z 채널에 선형 깊이가 들어있음

uniform float uFocusDistance;
uniform float uFocusRange;
uniform float uBokehRadius;
uniform vec2  uTexelSize;

const float PI = 3.141592;

void main()
{
	// [단계 1] 이미 계산된 선형 깊이 바로 읽기
	float linearDist = texture(uDepthTex, TexCoord).z;

	// [단계 2] CoC (착란원) 계산
	// 초점 거리와의 차이를 범위로 나누어 흐림 강도(0.0 ~ 1.0)를 구합니다.
	float coc = clamp(abs(linearDist - uFocusDistance) / uFocusRange, 0.0, 1.0);

	// [단계 3] 보케 블러 (Bokeh Blur) 샘플링
	vec3 centerColor = texture(uSceneTex, TexCoord).rgb;
    float blurAmount = coc * uBokehRadius;

    if (blurAmount < 0.05) 
    {
        fragColor = vec4(centerColor, 1.0);
        return;
    }

	// 더 부드러운 샘플링 (간이 보케 스타일)
    vec3 blurredColor = vec3(0.0);
    float totalWeight = 0.0;

	for (int i = 0; i < 32; i++)
    {
        float angle = i * (PI * 2.0 / 32.0);
        float radius = (float(i) / 32.0) * blurAmount; 
        
        vec2 offset = vec2(cos(angle), sin(angle)) * uTexelSize * radius;
        blurredColor += texture(uSceneTex, TexCoord + offset).rgb;
        totalWeight += 1.0;
    }

    vec3 finalColor = blurredColor / totalWeight;
    fragColor = vec4(finalColor, 1.0);
}