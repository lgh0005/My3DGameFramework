#include "GraphicsPch.h"
#include "StaticVertex.h"
#include "Utils/SubMeshBuilder.h"
#include "Layout/VertexLayout.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_ALIGNMENT(StaticVertex);

    template<>
    void SubMeshBuilder::SetupVertexLayout<StaticVertex>(Ptr<VertexLayout> layout, const GLVertexBuffer& vbo)
    {
        const usize stride = sizeof(StaticVertex);
        layout->SetAttrib(vbo, 0, 3, GL_FLOAT, false, stride, offsetof(StaticVertex, StaticVertex::position));
        layout->SetAttrib(vbo, 1, 3, GL_FLOAT, false, stride, offsetof(StaticVertex, StaticVertex::normal));
        layout->SetAttrib(vbo, 2, 2, GL_FLOAT, false, stride, offsetof(StaticVertex, StaticVertex::texCoord));
        layout->SetAttrib(vbo, 3, 3, GL_FLOAT, false, stride, offsetof(StaticVertex, StaticVertex::tangent));
    }
}