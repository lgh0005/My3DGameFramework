#include "EnginePch.h"
#include "SkinnedMesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffers/Buffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Layouts/VertexLayout.h"
#include "Resources/Programs/Program.h"

DECLARE_DEFAULTS_IMPL(SkinnedMesh)

SkinnedMeshUPtr SkinnedMesh::Create(const std::vector<SkinnedVertex>& vertices,
                                    const std::vector<uint32>& indices,
                                    uint32 primitiveType)
{
    auto mesh = SkinnedMeshUPtr(new SkinnedMesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void SkinnedMesh::Init(const std::vector<SkinnedVertex>& vertices,
                       const std::vector<uint32>& indices,
                       uint32 primitiveType)
{
    m_primitiveType = primitiveType;
    m_indexCount = indices.size();
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(SkinnedVertex), vertices.size());
    m_indexBuffer = IndexBuffer::Create(indices);

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(SkinnedVertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(SkinnedVertex), offsetof(SkinnedVertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(SkinnedVertex), offsetof(SkinnedVertex, texCoord));
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(SkinnedVertex), offsetof(SkinnedVertex, tangent));
    m_vertexLayout->SetAttribI(4, 4, GL_INT,         sizeof(SkinnedVertex), offsetof(SkinnedVertex, boneIDs));
    m_vertexLayout->SetAttrib(5, 4, GL_FLOAT, false, sizeof(SkinnedVertex), offsetof(SkinnedVertex, weights));
}

void SkinnedMesh::Draw() const
{
    m_vertexLayout->Bind();
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}
