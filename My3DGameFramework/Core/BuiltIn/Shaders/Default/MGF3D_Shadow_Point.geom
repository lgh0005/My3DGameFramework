#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out; // 삼각형 1개 * 6면 = 18정점

uniform mat4 shadowMatrices[6];
uniform int lightIndex;

// 프래그먼트 셰이더에서 거리를 계산할 수 있도록 월드 좌표를 넘겨줍니다.
out vec3 FragPos; 

void main() 
{
    for (int face = 0; face < 6; ++face) 
    {
        // 큐브맵 배열에서의 정확한 레이어 지정 (조명 번호 * 6 + 면 번호)
        gl_Layer = (lightIndex * 6) + face;

        for (int i = 0; i < 3; ++i) 
        {
            FragPos = gl_in[i].gl_Position.xyz;
            gl_Position = shadowMatrices[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}