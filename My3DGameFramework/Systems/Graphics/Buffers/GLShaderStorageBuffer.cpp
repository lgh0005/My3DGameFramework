#include "GraphicsPch.h"
#include "GLShaderStorageBuffer.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
    GLShaderStorageBuffer::GLShaderStorageBuffer() = default;
    GLShaderStorageBuffer::~GLShaderStorageBuffer() = default;

    GLShaderStorageBufferUPtr GLShaderStorageBuffer::Create(usize byteSize, VRAMAllocation::PoolType poolType)
    {
        auto buffer = GLShaderStorageBufferUPtr(new GLShaderStorageBuffer());
        if (!buffer->Init(byteSize, poolType)) return nullptr;
        return buffer;
    }

    bool GLShaderStorageBuffer::Init(usize byteSize, VRAMAllocation::PoolType poolType)
    {
        if (byteSize == 0) return false;

        m_allocation = MGF_VRAM.Allocate(poolType, byteSize, 256);
        if (!m_allocation.IsValid())
        {
            MGF_LOG_ERROR("GLShaderStorageBuffer: Failed to allocate VRAM.");
            return false;
        }

        return true;
    }

    void GLShaderStorageBuffer::Bind(uint32 ssboSlot) const
    {
        if (m_allocation.IsValid())
        {
            // 서브 할당된 VRAM 구간을 셰이더 스토리지 슬롯에 바인딩
            glBindBufferRange
            (
                GL_SHADER_STORAGE_BUFFER,
                ssboSlot,
                m_allocation.GetBufferHandle(),
                static_cast<GLintptr>(m_allocation.GetOffset()),
                static_cast<GLsizeiptr>(m_allocation.GetSize())
            );
        }
    }

    void GLShaderStorageBuffer::Unbind(uint32 ssboSlot) const
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboSlot, 0);
    }
}