#version 330 core  

/*============================================================================*/
// Reference Article here :                                                   //
// https://www.marginallyclever.com/2025/09/drawing-thick-outlines-in-opengl/ //
/*============================================================================*/

out vec4 finalColor;  

uniform sampler2D uSceneTexture;
uniform sampler2D stencilTexture; // 마스크 텍스처

uniform vec4 outlineColor;        // C++에서 설정 (기본값 설정은 셰이더 버전따라 다를 수 있음)
uniform float outlineSize;        // 두께
uniform vec2 canvasSize;          // 화면 해상도

void main() 
{  
    vec2 texelSize = 1.0 / canvasSize;  
    vec2 textureCoord = gl_FragCoord.xy / canvasSize;  
    
    // 1. 이전 패스의 장면 색상을 먼저 가져옴
    vec4 sceneColor = texture(uSceneTexture, textureCoord);
    vec4 stencilValue = texture(stencilTexture, textureCoord);  

    // 2. 물체 몸통 부분이면 아웃라인을 그리지 않고 원래 장면 출력
    if (stencilValue.r > 0.5) 
    {
        finalColor = sceneColor;
        return;
    }

    int outInt = int(ceil(outlineSize));  
    float o2 = outlineSize * outlineSize;  

    // 3. 주변 탐색 (외곽선 영역 확인)
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

    // 4. 아웃라인도 아니고 물체도 아니면 원래 장면 출력
    finalColor = sceneColor;
}