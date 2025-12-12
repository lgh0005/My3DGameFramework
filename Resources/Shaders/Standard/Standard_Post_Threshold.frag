#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D screenTexture; // 렌더링된 Scene Texture (HDR)

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // 밝기(Luma) 계산 (사람 눈 기준)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // 임계값(Threshold) 체크: 1.0보다 밝으면 통과, 아니면 검은색
    if (brightness > 1.0)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}