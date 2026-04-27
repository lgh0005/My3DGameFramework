#pragma once
#include "Meshes/Mesh.h"
#include "Resources/Material.h"

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
        if (batch.GetInstances().empty())
        {
            batch.SetMesh(mesh);
            batch.SetMaterial(material);
        }
        batch.GetInstances().push_back(data);
    }

    template<typename T>
    void RenderQueue<T>::Execute()
    {
        for (auto& [meshPtr, materialMap] : m_batches)
        {
            for (auto& [materialPtr, batch] : materialMap)
            {
                if (batch.GetInstances().empty()) continue;

                // 1. 버퍼 크기 체크 및 확장
                usize requiredSize = batch.GetInstances().size() * sizeof(T);
                if (m_instanceBuffer->GetByteSize() < requiredSize)
                {
                    usize newSize = static_cast<usize>(requiredSize * 1.5f);
                    m_instanceBuffer = GLShaderStorageBuffer::Create(nullptr, newSize);
                }

                // 2. 배치에게 "네 짐(Data) 들고 가서 그려라" 명령
                batch.Draw(m_bindingSlot, m_instanceBuffer.get());
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