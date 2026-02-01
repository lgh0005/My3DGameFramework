#include "EnginePch.h"
#include "RenderQueue.h"
#include "Instancing/InstanceBuffer.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Programs/Program.h"

DECLARE_DEFAULTS_IMPL(RenderQueue)

RenderQueueUPtr RenderQueue::Create(uint32 maxInstanceCount, uint32 maxBoneCount)
{
    auto queue = RenderQueueUPtr(new RenderQueue());
    if (!queue->Init(maxInstanceCount, maxBoneCount)) return nullptr;
    return std::move(queue);
}

bool RenderQueue::Init(uint32 maxInstanceCount, uint32 maxBoneCount)
{
    m_instanceBuffer = InstanceBuffer::Create(maxInstanceCount, sizeof(InstanceProperty));
    m_boneBuffer = InstanceBuffer::Create(maxBoneCount, sizeof(glm::mat4));
    return (m_instanceBuffer && m_boneBuffer);
}

void RenderQueue::Add(Mesh* mesh, Material* material, const InstanceProperty& prop, const std::vector<glm::mat4>& bones)
{
    if (!mesh || !material) return;

    auto& batch = m_batches[mesh][material];

    // 최초 배치 설정
    if (batch.mesh == nullptr)
    {
        batch.mesh = mesh;
        batch.material = material;
    }

    // 데이터 가공 및 삽입은 Batch의 로직을 따름 (여기서 typeFlag, boneOffset 결정)
    batch.Add(prop, bones);
}

void RenderQueue::Execute(Program* prog)
{
    if (m_batches.empty()) return;

    for (auto& [mesh, materialMap] : m_batches)
    {
        // [Optimization] 메쉬 바인딩 최소화
        mesh->Bind();

        for (auto& [material, batch] : materialMap)
        {
            if (batch.instanceData.empty()) continue;

            // 1. 머티리얼 적용
            material->SetToProgram(prog);

            // 2. 통합 인스턴스 데이터 전송 (Slot 0)
            m_instanceBuffer->Update(batch.instanceData.data(),
                static_cast<uint32>(batch.instanceData.size() * sizeof(InstanceProperty)));
            m_instanceBuffer->Bind(0);

            // 3. 뼈 데이터가 있는 배치의 경우만 전송 (Slot 1)
            if (!batch.boneData.empty())
            {
                m_boneBuffer->Update(batch.boneData.data(),
                    static_cast<uint32>(batch.boneData.size() * sizeof(glm::mat4)));
                m_boneBuffer->Bind(1);
            }

            // 4. 인스턴싱 드로우 콜
            mesh->RenderInstanced(static_cast<uint32>(batch.instanceData.size()));
        }
    }

    // 상태 복구
    m_instanceBuffer->Unbind(0);
    m_boneBuffer->Unbind(1);

    // VAO 및 EBO 해제 (사용자님의 TODO 반영)
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderQueue::Clear()
{
    // 맵 구조는 유지하되 내부 벡터 데이터만 비움 (Memory Reuse)
    for (auto& [mesh, materialMap] : m_batches)
    {
        for (auto& [material, batch] : materialMap)
        {
            batch.Clear();
        }
    }
}