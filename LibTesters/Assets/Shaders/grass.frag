#version 460 core

in vec2 texCoord;

layout (location = 0) out vec4 FragColor;   // 화면에 보일 색
layout (location = 1) out vec4 BrightColor; // Bloom 추출용 색 (여기다가 0을 넣어야 함)

uniform sampler2D tex;

void main() 
{
    vec4 pixel = texture(tex, texCoord);
    if (pixel.a < 0.05) discard;

    // 1. 화면 색상 (어둡게 테스트)
    FragColor = pixel;

    // 2. Bloom 마스크 (확실하게 0으로 꺼버림!)
    // 이걸 안 적으면 FragColor 값이 여기로 복사되어 버릴 수 있음 -> 발광 원인
    // !! 포워드 렌더링 패스가 PostProcess FBO의 PostProcess FBO(색상 버퍼 + 블룸 버퍼)를 공유해서 사용중 
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}