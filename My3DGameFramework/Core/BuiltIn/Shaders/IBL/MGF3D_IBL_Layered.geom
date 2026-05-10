#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 vLocalPos[];
out vec3 localPos;

// C++의 IBLData 구조체와 1:1 매칭
layout (std140, binding = 1) uniform IBLData 
{
    mat4 uViewProjections[6];
    float uRoughness;
    float padding[3];
};

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // 렌더링할 큐브맵의 면 지정 (0~5)
        for(int i = 0; i < 3; ++i)
        {
            localPos = vLocalPos[i];
            gl_Position = uViewProjections[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}