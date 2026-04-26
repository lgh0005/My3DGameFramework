#include "GraphicsPch.h"
#include "SkinnedMesh.h"
#include "CoreUtils/MathUtils.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Managers/TypeManager.h"
#include "Managers/ThreadManager.h"

namespace MGF3D
{
    SkinnedMesh::SkinnedMesh() = default;
    SkinnedMesh::~SkinnedMesh() = default;

    /*==========================//
    //   GLTextureHandle Type   //
    //==========================*/
    int16 SkinnedMesh::s_typeIndex = -1;
    const MGFType* SkinnedMesh::GetType() const
    {
        MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
        if (tree != nullptr) return tree->GetType(s_typeIndex);
        return nullptr;
    }

    SkinnedMeshPtr SkinnedMesh::Create
    (
        Vector<SkinnedVertex>&& vertices,
        Vector<uint32>&& indices,
        uint32 primitiveType
    )
    {
        // 1. 메인 스레드에서 즉시 빈 껍데기 객체 생성 및 반환
        auto mesh = SkinnedMeshPtr(new SkinnedMesh());
        mesh->m_primitiveType = primitiveType;
        mesh->m_indexCount = indices.size();

        // 메모리 소유권 이전 (이후 CPU 워커에서 사용)
        mesh->m_vertices = std::move(vertices);
        mesh->m_indices = std::move(indices);

        // 상태를 Loaded으로 설정
        mesh->SetState(EResourceState::Loaded);

        return mesh;
    }

    bool SkinnedMesh::OnSyncCreate()
    {
        if (m_vertices.empty() || m_indices.empty()) return false;

        // 2. 리소스 생성
        m_vertexLayout = GLVertexLayout::Create();
        m_vertexBuffer = GLVertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(SkinnedVertex));
        m_indexBuffer = GLIndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(uint32));

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

        // 4. 모든 속성 활성화
        for (uint32 i = 0; i <= 5; ++i) m_vertexLayout->EnableAttrib(i);

        // 4. GPU 업로드 완료 후 CPU 측 원본 메모리 즉각 해제
        m_vertices.clear();
        m_vertices.shrink_to_fit();
        m_indices.clear();
        m_indices.shrink_to_fit();

        m_state = EResourceState::Ready;
        return true;
    }
}
