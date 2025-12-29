#version 330 core  

/*============================================================================*/
// Reference Article here :                                                   //
// https://www.marginallyclever.com/2025/09/drawing-thick-outlines-in-opengl/ //
/*============================================================================*/

out vec4 finalColor;  

uniform vec4 outlineColor;        // C++에서 설정 (기본값 설정은 셰이더 버전따라 다를 수 있음)
uniform float outlineSize;        // 두께
uniform vec2 canvasSize;          // 화면 해상도
uniform sampler2D stencilTexture; // 마스크 텍스처

void main() 
{  
    vec2 texelSize = 1.0 / canvasSize;  
    
    // 버텍스 속성 무시하고 화면 좌표로 직접 계산
    vec2 textureCoord = gl_FragCoord.xy / canvasSize;  
    
    vec4 stencilValue = texture(stencilTexture, textureCoord);  

    // 1. 내 자리가 흰색(몸통)이면 생략
    if (stencilValue.r > 0.0) discard;  

    int outInt = int(ceil(outlineSize));  
    float o2 = outlineSize * outlineSize;  

    // 2. 주변 탐색 (둥근 외곽선)
    for (int y = -outInt; y <= outInt; y++) 
    {
        for (int x = -outInt; x <= outInt; x++) 
        {
            // 원형 범위 밖은 무시 (모서리 둥글게)
            if (x*x + y*y > o2) continue; 

            vec2 offset = vec2(x, y) * texelSize;  
            vec4 neighbor = texture(stencilTexture, textureCoord + offset);  
            
            // 주변에 흰색(몸통)이 하나라도 있으면 나는 외곽선
            if (neighbor.r > 0.5) 
            {  
                finalColor = outlineColor;  
                return;  
            }  
        }  
    }  

    // 3. 나머지는 버림
    discard;
}