#version 460 core

// 삼각형 하나(정점 3개)를 받아서
layout(triangles) in;
// 최대 12개의 정점(삼각형 1개 * 4개의 Cascade 층)을 내보냅니다.
layout(triangle_strip, max_vertices = 12) out; 

// C++에서 넘겨준 4개의 투영 행렬과 텍스처 배열의 시작 층(Layer)
uniform mat4 lightSpaceMatrices[4];
uniform int baseLayerIndex;

void main() 
{
    // 4개의 Cascade 분할 영역을 순회
    for (int cascade = 0; cascade < 4; ++cascade) 
    {
        // 핵심: OpenGL 내장 변수인 gl_Layer에 값을 넣으면, 
        // 바인딩된 GLTexture2DArray의 해당 층(Index)으로 렌더링 결과가 향합니다.
        gl_Layer = baseLayerIndex + cascade;

        // 삼각형의 3개 정점을 순회하며 분할 행렬 적용
        for (int i = 0; i < 3; ++i) 
        {
            // gl_in[i].gl_Position은 버텍스 셰이더에서 넘어온 월드 좌표입니다.
            gl_Position = lightSpaceMatrices[cascade] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive(); // 하나의 삼각형(하나의 레이어) 완성
    }
}