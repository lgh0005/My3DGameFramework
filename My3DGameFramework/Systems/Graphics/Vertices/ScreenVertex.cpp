#include "GraphicsPch.h"
#include "ScreenVertex.h"
#include "Utils/SubMeshBuilder.h"
#include "Layout/VertexLayout.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_ALIGNMENT(ScreenVertex);

    template<>
    void SubMeshBuilder::SetupVertexLayout<ScreenVertex>(Ptr<VertexLayout> layout, const GLVertexBuffer& vbo)
    {
        const usize stride = sizeof(ScreenVertex);
        layout->SetAttrib(vbo, 0, 3, GL_FLOAT, false, stride, offsetof(ScreenVertex, ScreenVertex::position));
        layout->SetAttrib(vbo, 1, 2, GL_FLOAT, false, stride, offsetof(ScreenVertex, ScreenVertex::texCoord));
    }
}