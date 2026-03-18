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

    void GLShaderStorageBuffer::Bind() const 
    {
        if (m_allocation.IsValid())
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_allocation.GetBufferHandle());
    }

    void GLShaderStorageBuffer::BindBase(uint32 slot) const 
    { 
        if (m_allocation.IsValid())
        {
            glBindBufferRange
            (
                GL_SHADER_STORAGE_BUFFER,
                slot,
                m_allocation.GetBufferHandle(),
                m_allocation.GetOffset(),
                m_allocation.GetSize()
            );
        }
    }

    void GLShaderStorageBuffer::Unbind() const 
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 
    }
}