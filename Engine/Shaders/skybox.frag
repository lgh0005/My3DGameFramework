#version 460 core

in vec3 texCoord;
layout (location = 0) out vec4 fragColor;    // 화면
layout (location = 1) out vec4 BrightColor;  // 밝은 영역

uniform samplerCube skybox;

void main() 
{
    vec3 color = texture(skybox, texCoord).rgb;
    fragColor = vec4(color, 1.0);

    // 3. 밝은 영역 추출 (MRT 필수!)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}