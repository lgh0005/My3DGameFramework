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
    // 0. 유효성 검사
    if (!mesh || !material) return;

    // 1. 해당 메쉬/머티리얼 조합의 배치를 가져오거나 생성 (없으면 자동 생성됨)
    auto& batch = m_batches[mesh][material];

    // 2. 처음 생성된 배치라면 메쉬와 머티리얼 정보 세팅
    if (batch.mesh == nullptr)
    {
        batch.mesh = mesh;
        batch.material = material;
    }

    // 3. 인스턴스 속성 추가
    // StaticMesh는 BoneOffset 연산이 필요 없으므로 바로 추가합니다.
    batch.Add(prop);
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
        for (auto& [material, batch] : materialMap)
        {
            if (batch.instanceData.empty()) continue;

            // 머티리얼 적용
            material->SetToProgram(prog);

            // 데이터 업데이트 및 드로우 콜
            m_instanceBuffer->Update(batch.instanceData.data(), 
                static_cast<uint32>(batch.instanceData.size()) * sizeof(StaticInstanceProperty));
            m_instanceBuffer->Bind(0);

            // 4. 인스턴싱 드로우 콜 수행
            mesh->RenderInstanced(static_cast<uint32>(batch.instanceData.size()));
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
        for (auto& [material, batch] : materialMap)
            batch.Clear();
    }
}