#include "EnginePch.h"
#include "InstancedMesh.h"
#include "Graphics/VertexLayout.h"
#include "Resources/Material.h"
#include "Resources/StaticMesh.h"
#include "Graphics/Buffer.h"

DECLARE_DEFAULTS_IMPL(InstancedMesh)

InstancedMeshUPtr InstancedMesh::Create(StaticMeshPtr baseMesh, BufferPtr instanceBuffer, 
                                        int32 instanceCount, SetupFunc setupFunc)
{
    auto renderer = InstancedMeshUPtr(new InstancedMesh());
    if (!renderer->Init(baseMesh, instanceBuffer, instanceCount, setupFunc)) return nullptr;
    return std::move(renderer);
}

void InstancedMesh::Draw() const
{
    m_vertexLayout->Bind();
    glDrawElementsInstanced
    (
        m_primitiveType,
        (GLsizei)m_indexBuffer->GetCount(),
        GL_UNSIGNED_INT,
        0,
        (GLsizei)m_instanceCount
    );
}

bool InstancedMesh::Init(StaticMeshPtr baseMesh, BufferPtr instanceBuffer, 
                         int32 instanceCount, SetupFunc setupFunc)
{
    // TODO : 현재는 StaticMesh를 전제로 인스턴싱을 수행.
    // 이후에는 "다른 애니메이션에 같은 모델"을 인스턴싱 해야 할 수도 있음.
    // 1. 부모의 멤버를 baseMesh의 것으로 설정 (포인터 공유)
    // 2. 기회가 된다면 SSBO를 이용한 "애니메이션 모델 인스턴싱"도 이론적으로 가능함.
    m_vertexBuffer = baseMesh->GetVertexBuffer();
    m_indexBuffer = baseMesh->GetIndexBuffer();
    m_indexCount = baseMesh->GetIndexCount();
    m_primitiveType = GL_TRIANGLES;

    // 2. 인스턴스 데이터는 직접 소유
    m_instanceBuffer = instanceBuffer;
    m_instanceCount = instanceCount;

    // 3. InstancedMesh만의 "하이브리드 VAO" 생성
    m_vertexLayout = VertexLayout::Create();
    m_vertexLayout->Bind();

    // 4. 원본 메쉬(Per-Vertex)의 VBO/IBO 바인딩 및 속성 설정
    m_vertexBuffer->Bind();
    m_indexBuffer->Bind();
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, position));
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, texCoord));

    m_instanceBuffer->Bind();

    uint32 instanceAttribStartIndex = 3;
    setupFunc(m_vertexLayout.get(), instanceAttribStartIndex);

    return true;
}

/*=========================//
//  instance method utils  //
//=========================*/
#pragma region INSTANCE_METHOD_UTILS

InstancedMesh::SetupFunc InstancedMesh::Vec3Layout()
{
    return [](VertexLayout* layout, uint32 startIndex)
        {
            layout->SetAttrib(startIndex, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
            layout->SetAttribDivisor(startIndex, 1);
        };
}

InstancedMesh::SetupFunc InstancedMesh::Mat4Layout()
{
    return [](VertexLayout* layout, uint32 startIndex)
        {
            usize vec4Size = sizeof(glm::vec4);

            // mat4는 vec4 4개로 쪼개서 설정
            layout->SetAttrib(startIndex + 0, 4, GL_FLOAT, false, sizeof(glm::mat4), vec4Size * 0);
            layout->SetAttrib(startIndex + 1, 4, GL_FLOAT, false, sizeof(glm::mat4), vec4Size * 1);
            layout->SetAttrib(startIndex + 2, 4, GL_FLOAT, false, sizeof(glm::mat4), vec4Size * 2);
            layout->SetAttrib(startIndex + 3, 4, GL_FLOAT, false, sizeof(glm::mat4), vec4Size * 3);

            // 4개 속성 모두 Divisor를 1로 설정
            layout->SetAttribDivisor(startIndex + 0, 1);
            layout->SetAttribDivisor(startIndex + 1, 1);
            layout->SetAttribDivisor(startIndex + 2, 1);
            layout->SetAttribDivisor(startIndex + 3, 1);
        };
}

#pragma endregion