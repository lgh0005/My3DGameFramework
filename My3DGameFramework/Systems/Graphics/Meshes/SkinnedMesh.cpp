#include "GraphicsPch.h"
#include "SkinnedMesh.h"
#include "CoreUtils/MathUtils.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"

namespace MGF3D
{
    SkinnedMesh::SkinnedMesh() = default;
    SkinnedMesh::~SkinnedMesh() = default;

    SkinnedMeshPtr SkinnedMesh::Create
    (
        const Vector<SkinnedVertex>& vertices,
        const Vector<uint32>& indices,
        uint32 primitiveType
    )
    {
        auto mesh = SkinnedMeshPtr(new SkinnedMesh());
        mesh->Init(vertices, indices, primitiveType);
        return mesh;
    }

    void SkinnedMesh::Init
    (
        const Vector<SkinnedVertex>& vertices,
        const Vector<uint32>& indices,
        uint32 primitiveType
    )
    {
        // 0. 기본 메쉬 설정
        m_primitiveType = primitiveType;
        m_indexCount = indices.size();

        // 1. 탄젠트 연산
        if (m_primitiveType == GL_TRIANGLES)
            ComputeTangents(const_cast<Vector<SkinnedVertex>&>(vertices), indices);

       // 2. 리소스 생성
        m_vertexLayout = GLVertexLayout::Create();
        m_vertexBuffer = GLVertexBuffer::Create(vertices.data(), vertices.size() * sizeof(SkinnedVertex));
        m_indexBuffer = GLIndexBuffer::Create(indices.data(), indices.size() * sizeof(uint32));

        // 3. DSA 레이아웃 설정
        const uint32 bindingIndex = 0;
        m_vertexLayout->BindVertexBuffer(bindingIndex, m_vertexBuffer, 0, sizeof(SkinnedVertex));
        m_vertexLayout->BindIndexBuffer(m_indexBuffer);
        m_vertexLayout->SetAttribFormat(0, 3, GL_FLOAT, false, offsetof(SkinnedVertex, position), bindingIndex);
        m_vertexLayout->SetAttribFormat(1, 3, GL_FLOAT, false, offsetof(SkinnedVertex, normal), bindingIndex);
        m_vertexLayout->SetAttribFormat(2, 2, GL_FLOAT, false, offsetof(SkinnedVertex, texCoord), bindingIndex);
        m_vertexLayout->SetAttribFormat(3, 3, GL_FLOAT, false, offsetof(SkinnedVertex, tangent), bindingIndex);
        m_vertexLayout->SetAttribIFormat(4, 4, GL_INT, offsetof(SkinnedVertex, boneIDs), bindingIndex);
        m_vertexLayout->SetAttribFormat(5, 4, GL_FLOAT, false, offsetof(SkinnedVertex, weights), bindingIndex);
        for (uint32 i = 0; i <= 5; ++i) m_vertexLayout->EnableAttrib(i);
    }

    void SkinnedMesh::ComputeTangents
    (
        Vector<SkinnedVertex>& vertices,
        const Vector<uint32>& indices
    )
    {
        Vector<vec3> tangents;
        tangents.resize(vertices.size(), vec3(0.0f));

        for (usize i = 0; i < indices.size(); i += 3)
        {
            int32 i0 = indices[i];
            uint32 i1 = indices[i + 1];
            uint32 i2 = indices[i + 2];

            const auto& v0 = vertices[i0];
            const auto& v1 = vertices[i1];
            const auto& v2 = vertices[i2];

            // Edge(변) 벡터
            vec3 edge1 = v1.position - v0.position;
            vec3 edge2 = v2.position - v0.position;

            // Delta UV 벡터
            vec2 deltaUV1 = v1.texCoord - v0.texCoord;
            vec2 deltaUV2 = v2.texCoord - v0.texCoord;

            // 행렬식(Determinant) 계산
            float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            if (det != 0.0f)
            {
                float invDet = 1.0f / det;

                // 삼각형의 Tangent는 하나입니다!
                vec3 tangent;
                tangent.x = invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

                // 구해진 하나의 Tangent를 세 정점에 모두 더해줍니다.
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
            if (Math::HasLength(tangents[i])) vertices[i].tangent = normalize(tangents[i]);
            else vertices[i].tangent = vec3(1.0f, 0.0f, 0.0f);
        }
    }

    void SkinnedMesh::Draw(uint32 count) const
    {
        Bind();
        Super::Draw(count);
        Unbind();
    }
}
