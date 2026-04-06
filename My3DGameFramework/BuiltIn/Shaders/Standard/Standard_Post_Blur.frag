#version 460 core

layout (location = 0) in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D image;      // 블러 처리할 텍스쳐 (밝은 영역 or 방금 블러된 텍스쳐)
uniform bool horizontal;      // 가로(true) / 세로(false) 방향 결정

// 가우시안 가중치 (총합이 1.0에 근접하는 5개의 샘플 가중치)
// 중심(0번)이 가장 크고, 멀어질수록 작아집니다.
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    // 1. 텍셀(픽셀) 하나의 크기를 계산합니다. (1.0 / 해상도)
    // 예: 800px 너비라면 1/800 = 0.00125
    vec2 tex_offset = 1.0 / textureSize(image, 0); 
    
    // 2. 현재 픽셀(중심)의 색상 * 가중치
    vec3 result = texture(image, texCoord).rgb * weight[0]; 
    
    if (horizontal)
    {
        // [가로 모드] 좌우로 4칸씩 이동하며 샘플링
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i]; // 오른쪽 픽셀
            result += texture(image, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i]; // 왼쪽 픽셀
        }
    }
    else
    {
        // [세로 모드] 상하로 4칸씩 이동하며 샘플링
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i]; // 위쪽 픽셀
            result += texture(image, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i]; // 아래쪽 픽셀
        }
    }
    
    // 알파는 1.0으로 고정 (Bloom은 빛이니까 투명도는 의미 없음)
    fragColor = vec4(result, 1.0);
}