#include "EnginePch.h"
#include "SkinnedMesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Program.h"

SkinnedMesh::SkinnedMesh() = default;
SkinnedMesh::~SkinnedMesh() = default;

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
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData
    (
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(SkinnedVertex), vertices.size()
    );
    m_indexBuffer = Buffer::CreateWithData
    (
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32), indices.size()
    );
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
