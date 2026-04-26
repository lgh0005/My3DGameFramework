#pragma once

namespace MGF3D
{
    template<typename T>
    RenderQueue<T>::RenderQueue(uint32 bindingSlot, usize initialCapacity) : m_bindingSlot(bindingSlot)
    {
        m_instanceBuffer = GLShaderStorageBuffer::Create(nullptr, initialCapacity * sizeof(T));
    }

    template<typename T> RenderQueue<T>::~RenderQueue() = default;

    template<typename T>
    void RenderQueue<T>::Submit(Mesh* mesh, Material* material, const T& data)
    {
        if (!mesh || !material) return;

        auto& batch = m_batches[mesh][material];
        if (batch.instances.empty())
        {
            batch.mesh = mesh;
            batch.material = material;
        }
        batch.instances.push_back(data);
    }

    template<typename T>
    void RenderQueue<T>::Execute()
    {
        for (auto& [meshPtr, materialMap] : m_batches)
        {
            // 1. 메쉬 바인딩 (VAO)
            meshPtr->Bind();

            for (auto& [materialPtr, batch] : materialMap)
            {
                if (batch.instances.empty()) continue;

                // 2. 머티리얼 바인딩 (Shader, UBO, Textures)
                materialPtr->Bind();

                // 3. 인스턴스 데이터 전송 (SSBO)
                usize requiredSize = batch.instances.size() * sizeof(T);

                // 버퍼 크기가 부족하면 확장 (Growth Strategy)
                if (m_instanceBuffer->GetByteSize() < requiredSize)
                {
                    // 1.5배수로 넉넉하게 재할당하여 빈번한 Stall 방지
                    usize newSize = static_cast<usize>(requiredSize * 1.5f);
                    m_instanceBuffer = GLShaderStorageBuffer::Create(nullptr, newSize);
                }

                // DSA를 통한 고속 데이터 업로드
                m_instanceBuffer->UpdateData(batch.instances.data(), 0, requiredSize);

                // 지정된 슬롯(예: 1번)에 SSBO 바인딩
                m_instanceBuffer->Bind(m_bindingSlot);

                // 4. 인스턴싱 드로우 콜 실행
                meshPtr->RenderInstanced(static_cast<uint32>(batch.instances.size()));
            }
        }
    }

    template<typename T>
    void RenderQueue<T>::Clear()
    {
        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
                batch.GetInstances().clear();
        }
    }
}