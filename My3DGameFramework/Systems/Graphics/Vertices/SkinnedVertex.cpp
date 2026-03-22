#include "GraphicsPch.h"
#include "SkinnedVertex.h"
#include "Utils/SubMeshBuilder.h"
#include "Layouts/VertexLayout.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_ALIGNMENT(SkinnedVertex);

    template<>
    void SubMeshBuilder::SetupVertexLayout<SkinnedVertex>(Ptr<VertexLayout> layout, const GLVertexBuffer& vbo)
    {
        const usize stride = sizeof(SkinnedVertex);
        layout->SetAttrib(vbo, 0, 3, GL_FLOAT, false, stride, offsetof(SkinnedVertex, SkinnedVertex::position));
        layout->SetAttrib(vbo, 1, 3, GL_FLOAT, false, stride, offsetof(SkinnedVertex, SkinnedVertex::normal));
        layout->SetAttrib(vbo, 2, 2, GL_FLOAT, false, stride, offsetof(SkinnedVertex, SkinnedVertex::texCoord));
        layout->SetAttrib(vbo, 3, 3, GL_FLOAT, false, stride, offsetof(SkinnedVertex, SkinnedVertex::tangent));
        layout->SetAttribI(vbo, 4, 4, GL_INT, stride, offsetof(SkinnedVertex, SkinnedVertex::boneIDs));
        layout->SetAttrib(vbo, 5, 4, GL_FLOAT, false, stride, offsetof(SkinnedVertex, SkinnedVertex::weights));
    }
}