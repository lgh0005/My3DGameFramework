#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D mainTexture;
uniform vec2 texelSize; // (1.0 / width, 1.0 / height)
uniform int mode;       // 0: Prefilter, 1: Downsample, 2: Upsample
uniform float threshold; // 밝기 임계값 (보통 1.0 ~ 1.5)

// 1. Prefilter: 밝은 색상만 추출
vec3 Prefilter(vec3 color)
{
    // 픽셀의 밝기(Luma) 계산 (사람 눈에 민감한 Green에 가중치)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Threshold보다 밝은 부분만 남김
    if(brightness > threshold)
        return color;
    else
        return vec3(0.0);
}

// 2. Downsample: 13-Tap Dual Filter (Box + Cross)
// 이미지를 줄일 때 픽셀을 고르게 섞어줍니다.
vec3 Downsample()
{
    vec3 color = vec3(0.0);
    
    // 텍셀의 중심이 아니라 사이사이를 찍어서 Bilinear Filtering을 유도
    vec2 halfTexel = texelSize * 0.5;

    // Center (4개의 픽셀 평균)
    vec3 c  = texture(mainTexture, TexCoords).rgb;
    
    // Outer Box (주변부 4개)
    vec3 tl = texture(mainTexture, TexCoords + vec2(-2, -2) * texelSize).rgb;
    vec3 tr = texture(mainTexture, TexCoords + vec2( 2, -2) * texelSize).rgb;
    vec3 bl = texture(mainTexture, TexCoords + vec2(-2,  2) * texelSize).rgb;
    vec3 br = texture(mainTexture, TexCoords + vec2( 2,  2) * texelSize).rgb;
    
    // 가중치 계산 (Center에 힘을 더 실어줌)
    // CoD Dual Filter 방식
    color = c * 0.5;
    color += (tl + tr + bl + br) * 0.125;
    
    return color;
}

// 3. Upsample: 3x3 Tent Filter
// 이미지를 키울 때 깍두기 현상 없이 부드럽게 펴줍니다.
vec3 Upsample()
{
    // 필터 반경 (Radius)
    float x = texelSize.x;
    float y = texelSize.y;

    // 9-Tap Tent Filter weights
    // 1 2 1
    // 2 4 2
    // 1 2 1
    
    vec3 result = vec3(0.0);
    
    result += texture(mainTexture, TexCoords + vec2(-x,  y)).rgb * 1.0;
    result += texture(mainTexture, TexCoords + vec2( 0,  y)).rgb * 2.0;
    result += texture(mainTexture, TexCoords + vec2( x,  y)).rgb * 1.0;
    
    result += texture(mainTexture, TexCoords + vec2(-x,  0)).rgb * 2.0;
    result += texture(mainTexture, TexCoords + vec2( 0,  0)).rgb * 4.0;
    result += texture(mainTexture, TexCoords + vec2( x,  0)).rgb * 2.0;
    
    result += texture(mainTexture, TexCoords + vec2(-x, -y)).rgb * 1.0;
    result += texture(mainTexture, TexCoords + vec2( 0, -y)).rgb * 2.0;
    result += texture(mainTexture, TexCoords + vec2( x, -y)).rgb * 1.0;
    
    return result * (1.0 / 16.0); // 가중치 합으로 나눔
}

void main()
{
     vec3 result = vec3(0.0);

    switch(mode)
    {
        case 0:
            vec3 color = Downsample(); 
            result = Prefilter(color);
            break;

        case 1: // Downsample
            result = Downsample();
            break;

        case 2: // Upsample
            result = Upsample();
            break;
    }

     FragColor = vec4(result, 1.0);
}