#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput; // Raw SSAO Texture

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    
    // -2 ~ +2 범위의 4x4 영역 평균 계산
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    
    FragColor = result / 16.0;
}