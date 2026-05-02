#pragma once
#include "Buffers/GLShaderStorageBuffer.h"

namespace MGF3D
{
    template <typename T>
    void RenderContext::UpdateSSBO
    (
        GLShaderStorageBufferUPtr& buffer, 
        const Vector<T>& data, 
        uint32 bindingIndex
    )
    {
        if (data.empty()) return;

        usize size = data.size() * sizeof(T);

        // 포인터가 유효하지 않거나 버퍼 크기가 작으면 새로 생성
        if (!buffer || buffer->GetByteSize() < size)
            buffer = GLShaderStorageBuffer::Create(nullptr, size);

        buffer->UpdateData(data.data(), 0, size);
        buffer->Bind(bindingIndex);
    }
}