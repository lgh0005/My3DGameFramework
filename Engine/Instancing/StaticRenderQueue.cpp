#include "EnginePch.h"
#include "StaticRenderQueue.h"
#include "Instancing/InstanceBuffer.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Material.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"

DECLARE_DEFAULTS_IMPL(StaticRenderQueue)

StaticRenderQueueUPtr StaticRenderQueue::Create(uint32 maxInstanceCount)
{
    auto queue = StaticRenderQueueUPtr(new StaticRenderQueue());
    if (!queue->Init(maxInstanceCount)) return nullptr;
    return std::move(queue);
}

bool StaticRenderQueue::Init(uint32 maxInstanceCount)
{
    m_instanceBuffer = InstanceBuffer::Create(maxInstanceCount, sizeof(StaticInstanceProperty));
    if (!m_instanceBuffer) return false;
    return true;
}

void StaticRenderQueue::Add(Mesh* mesh, Material* material, const StaticInstanceProperty& prop)
{
    if (!mesh || !material) return;
    m_batches[mesh][material].push_back(prop);
}

void StaticRenderQueue::Execute(Program* prog)
{
    if (m_batches.empty()) return;

    // [Step 1] Mesh 단위 순회 (가장 바깥쪽 루프)
    for (auto& [mesh, materialMap] : m_batches)
    {
        // 동일 메쉬를 사용하는 모든 배치를 위해 단 한 번만 바인딩
        mesh->Bind();

        // [Step 2] Material 단위 순회 (안쪽 루프)
        for (auto& [material, instanceList] : materialMap)
        {
            if (instanceList.empty()) continue;

            // 1. 머티리얼 속성 전송 (Texture, Uniforms)
            material->SetToProgram(prog);

            // 2. 인스턴싱 데이터 업데이트 (CPU -> GPU)
            uint32 instanceCount = static_cast<uint32>(instanceList.size());
            m_instanceBuffer->Update(instanceList.data(), instanceCount * sizeof(StaticInstanceProperty));

            // 3. 인스턴스 버퍼 바인딩 (layout binding = 0)
            m_instanceBuffer->Bind(0);

            // 4. 인스턴싱 드로우 콜 수행
            mesh->RenderInstanced(instanceCount);
        }
    }

    m_instanceBuffer->Unbind(0);

    // TODO : 이거 각각 static으로 감싸야 할 필요가 있을 듯
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void StaticRenderQueue::Clear()
{
    for (auto& [mesh, materialMap] : m_batches)
    {
        for (auto& [material, instanceList] : materialMap)
            instanceList.clear();
    }
}