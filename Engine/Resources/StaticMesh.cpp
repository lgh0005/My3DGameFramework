#include "EnginePch.h"
#include "StaticMesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Program.h"

StaticMesh::StaticMesh() = default;
StaticMesh::~StaticMesh() = default;

StaticMeshUPtr StaticMesh::Create(const std::vector<StaticVertex>& vertices,
	                              const std::vector<uint32>& indices,
	                              uint32 primitiveType)
{
	auto mesh = StaticMeshUPtr(new StaticMesh());
	mesh->Init(vertices, indices, primitiveType);
	return std::move(mesh);
}

void StaticMesh::Init(const std::vector<StaticVertex>& vertices,
                      const std::vector<uint32>& indices,
                      uint32 primitiveType)
{
    m_primitiveType = primitiveType;
    if (m_primitiveType == GL_TRIANGLES)
        ComputeTangents(const_cast<std::vector<StaticVertex>&>(vertices), indices);

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData
    (
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(StaticVertex), vertices.size()
    );
    m_indexBuffer = Buffer::CreateWithData
    (
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32), indices.size()
    );
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(StaticVertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, texCoord));
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, tangent));
}

void StaticMesh::ComputeTangents(std::vector<StaticVertex>& vertices, const std::vector<uint32>& indices)
{
    std::vector<glm::vec3> tangents;
    tangents.resize(vertices.size(), glm::vec3(0.0f));

    for (usize i = 0; i < indices.size(); i += 3)
    {
        int32 i0 = indices[i];
        uint32 i1 = indices[i + 1];
        uint32 i2 = indices[i + 2];

        const auto& v0 = vertices[i0];
        const auto& v1 = vertices[i1];
        const auto& v2 = vertices[i2];

        // Edge(변) 벡터
        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        // Delta UV 벡터
        glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
        glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

        // 행렬식(Determinant) 계산
        float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        if (det != 0.0f)
        {
            float invDet = 1.0f / det;

            // [핵심] 삼각형의 Tangent는 하나입니다!
            glm::vec3 tangent;
            tangent.x = invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            // [누적] 구해진 하나의 Tangent를 세 정점에 모두 더해줍니다.
            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;
        }
        else
        {
            tangents[i0] += edge1;
            tangents[i1] += edge1;
            tangents[i2] += edge1;
        }
    }

    // 3. 정규화 (Normalize) - 평균낸 방향을 단위 벡터로 만듦
    for (usize i = 0; i < vertices.size(); i++)
    {
        if (glm::length(tangents[i]) > glm::epsilon<float>())
        {
            vertices[i].tangent = glm::normalize(tangents[i]);
        }
        else
        {
            vertices[i].tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }
}

void StaticMesh::Draw() const
{
    m_vertexLayout->Bind();
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}