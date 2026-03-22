#include "GraphicsPch.h"
#include "UIVertex.h"
#include "Utils/SubMeshBuilder.h"
#include "Layouts/VertexLayout.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_ALIGNMENT(UIVertex);
    MGF_ASSERT_GPU_OFFSET(UIVertex, color, 16);

    template<>
    void SubMeshBuilder::SetupVertexLayout<UIVertex>(Ptr<VertexLayout> layout, const GLVertexBuffer& vbo)
    {
        const usize stride = sizeof(UIVertex);
        layout->SetAttrib(vbo, 0, 3, GL_FLOAT, false, stride, offsetof(UIVertex, UIVertex::position));
        layout->SetAttrib(vbo, 1, 4, GL_FLOAT, false, stride, offsetof(UIVertex, UIVertex::color));
        layout->SetAttrib(vbo, 2, 2, GL_FLOAT, false, stride, offsetof(UIVertex, UIVertex::texCoord));
    }
}