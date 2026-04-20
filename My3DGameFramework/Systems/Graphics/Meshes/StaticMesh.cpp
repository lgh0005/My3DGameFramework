#include "GraphicsPch.h"
#include "StaticMesh.h"
#include "CoreUtils/MathUtils.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Managers/TypeManager.h"
#include "Managers/ThreadManager.h"

namespace MGF3D
{
    StaticMesh::StaticMesh() = default;
    StaticMesh::~StaticMesh() = default;

    /*==========================//
    //   GLTextureHandle Type   //
    //==========================*/
    int16 StaticMesh::s_typeIndex = -1;
    const MGFType* StaticMesh::GetType() const
    {
        MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
        if (tree != nullptr) return tree->GetType(s_typeIndex);
        return nullptr;
    }

    StaticMeshPtr StaticMesh::Create
    (
        Vector<StaticVertex>&& vertices,
        Vector<uint32>&& indices,
        uint32 primitiveType
    )
    {
        // 1. 메인 스레드에서 즉시 빈 껍데기 객체 생성 및 반환
        auto mesh = StaticMeshPtr(new StaticMesh());
        mesh->m_primitiveType = primitiveType;
        mesh->m_indexCount = indices.size();

        // 메모리 소유권 이전 (이후 CPU 워커에서 사용)
        mesh->m_vertices = std::move(vertices);
        mesh->m_indices = std::move(indices);

        // 상태를 Loading으로 설정 (아직 그릴 수 없음)
        mesh->SetState(EResourceState::Loading);

        // 2. CPU 헤비 연산을 백그라운드 워커에 할당
        MGF_THREAD.PushCPUTask
        (
            [mesh]()
            {
                if (mesh->m_primitiveType == GL_TRIANGLES)
                    mesh->ComputeTangents(mesh->m_vertices, mesh->m_indices);
                
                mesh->SetLocalBounds(RenderBounds(mesh->m_vertices));
                mesh->SetState(EResourceState::Loaded);

                // 2. [GPU 워커 스레드] 연쇄 할당
                MGF_THREAD.PushGPUTask
                (
                    [mesh]()
                    {
                        mesh->SetState(EResourceState::Syncing);

                        if (mesh->OnSyncCreate()) mesh->SetState(EResourceState::Ready);
                        else mesh->SetState(EResourceState::Failed);
                    }
                );
            }
        );

        return mesh;
    }

    bool StaticMesh::OnSyncCreate()
    {
        if (m_vertices.empty() || m_indices.empty()) return false;

        // 1. 리소스 생성
        m_vertexLayout = GLVertexLayout::Create();
        m_vertexBuffer = GLVertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(StaticVertex));
        m_indexBuffer = GLIndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(uint32));

        // 2. DSA 레이아웃 설정
        const uint32 bindingIndex = 0;
        m_vertexLayout->BindVertexBuffer(bindingIndex, m_vertexBuffer, 0, sizeof(StaticVertex));
        m_vertexLayout->BindIndexBuffer(m_indexBuffer);
        m_vertexLayout->SetAttribFormat(0, 3, GL_FLOAT, false, offsetof(StaticVertex, position), bindingIndex);
        m_vertexLayout->SetAttribFormat(1, 3, GL_FLOAT, false, offsetof(StaticVertex, normal), bindingIndex);
        m_vertexLayout->SetAttribFormat(2, 2, GL_FLOAT, false, offsetof(StaticVertex, texCoord), bindingIndex);
        m_vertexLayout->SetAttribFormat(3, 3, GL_FLOAT, false, offsetof(StaticVertex, tangent), bindingIndex);

        // 3. 모든 속성 활성화
        for (uint32 i = 0; i <= 3; ++i) m_vertexLayout->EnableAttrib(i);

        // 4. GPU 업로드 완료 후 CPU 측 원본 메모리 즉각 해제
        m_vertices.clear();
        m_vertices.shrink_to_fit();
        m_indices.clear();
        m_indices.shrink_to_fit();

        m_state = EResourceState::Ready;
        return true;
    }

    void StaticMesh::ComputeTangents
    (
        Vector<StaticVertex>& vertices, 
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

    void StaticMesh::Draw(uint32 count) const
    {
        Bind();
        Super::Draw(count);
        Unbind();
    }
}