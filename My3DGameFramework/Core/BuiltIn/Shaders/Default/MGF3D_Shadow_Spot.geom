#version 460 core

layout(triangles) in;
// 스팟 라이트는 단일 뷰이므로 삼각형 1개(정점 3개)만 내보냅니다.
layout(triangle_strip, max_vertices = 3) out; 

uniform mat4 lightSpaceMatrix;
uniform int layerIndex; // 텍스처 배열(GLTexture2DArray)에서의 층 번호

void main() 
{
    // 💡 대상 텍스처 레이어 지정
    gl_Layer = layerIndex;

    for (int i = 0; i < 3; ++i) 
    {
        // 월드 좌표에 스팟 라이트의 뷰-프로젝션 행렬을 곱하여 빛의 NDC 공간으로 변환
        gl_Position = lightSpaceMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}