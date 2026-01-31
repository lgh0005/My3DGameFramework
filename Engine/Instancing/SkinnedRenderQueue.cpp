#include "EnginePch.h"
#include "SkinnedRenderQueue.h"
#include "Instancing/InstanceBuffer.h"
#include "Resources/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"

DECLARE_DEFAULTS_IMPL(SkinnedRenderQueue)

SkinnedRenderQueueUPtr SkinnedRenderQueue::Create(uint32 maxInstanceCount, uint32 maxBoneCount)
{
    auto queue = SkinnedRenderQueueUPtr(new SkinnedRenderQueue());
    if (!queue->Init(maxInstanceCount, maxBoneCount)) return nullptr;
    return std::move(queue);
}

bool SkinnedRenderQueue::Init(uint32 maxInstanceCount)
{
    return Init(maxInstanceCount, maxInstanceCount * MAX_BONES);
}

bool SkinnedRenderQueue::Init(uint32 maxInstanceCount, uint32 maxBoneCount)
{
    m_instanceBuffer = InstanceBuffer::Create(maxInstanceCount, sizeof(SkinnedInstanceProperty));
    m_boneBuffer = InstanceBuffer::Create(maxBoneCount, sizeof(glm::mat4));
    return (m_instanceBuffer && m_boneBuffer);
}

void SkinnedRenderQueue::Add
(
    SkinnedMesh* mesh, Material* material, 
    SkinnedInstanceProperty prop, 
    const std::vector<glm::mat4>& boneMatrices
)
{
    if (!mesh || !material) return;

    auto& batch = m_batches[mesh][material];

    // 처음 생성된 배치라면 메쉬/머티리얼 정보 세팅
    if (batch.mesh == nullptr) 
    {
        batch.mesh = mesh;
        batch.material = material;
    }

    // [중요] 현재 배치에 이미 쌓인 뼈들의 개수가 이 인스턴스의 시작 위치(offset)가 됩니다.
    // 이 정보가 셰이더의 SSBO 인덱싱 기준이 됩니다.
    prop.boneOffset = static_cast<uint32>(batch.boneData.size());

    batch.Add(prop);
    // 인스턴스의 뼈 행렬들을 배치의 거대 데이터에 병합
    batch.boneData.insert(batch.boneData.end(), boneMatrices.begin(), boneMatrices.end());
}

void SkinnedRenderQueue::Execute(GraphicsProgram* prog)
{
    if (m_batches.empty()) return;

    for (auto& [mesh, materialMap] : m_batches)
    {
        mesh->Bind();

        for (auto& [material, batch] : materialMap)
        {
            if (batch.instanceData.empty()) continue;

            material->SetToProgram(prog);

            // 1. 인스턴스 속성 업데이트 및 바인딩 (Slot 0)
            m_instanceBuffer->Update(batch.instanceData.data(),
                static_cast<uint32>(batch.instanceData.size() * sizeof(SkinnedInstanceProperty)));
            m_instanceBuffer->Bind(0);

            // 2. 뼈 행렬 데이터 업데이트 및 바인딩 (Slot 1)
            // 현재 배치에 속한 모든 인스턴스의 뼈 데이터가 한꺼번에 전송됩니다.
            m_boneBuffer->Update(batch.boneData.data(),
                static_cast<uint32>(batch.boneData.size() * sizeof(glm::mat4)));
            m_boneBuffer->Bind(1);

            // 3. 드로우 콜
            mesh->RenderInstanced(static_cast<uint32>(batch.instanceData.size()));
        }
    }
}

void SkinnedRenderQueue::Clear()
{
    for (auto& [mesh, matMap] : m_batches)
    {
        for (auto& [mat, batch] : matMap)
        {
            batch.Clear();
            batch.boneData.clear(); // 뼈 데이터도 잊지 말고 비워줍니다.
        }
    }
}