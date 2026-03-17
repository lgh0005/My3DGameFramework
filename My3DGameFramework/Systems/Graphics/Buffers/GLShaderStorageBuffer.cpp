#include "GraphicsPch.h"
#include "GLShaderStorageBuffer.h"

namespace MGF3D
{
    GLShaderStorageBuffer::GLShaderStorageBuffer() = default;
    GLShaderStorageBuffer::~GLShaderStorageBuffer() = default;

    GLShaderStorageBufferUPtr GLShaderStorageBuffer::Create(usize byteSize, uint32 flags)
    {
        auto buffer = GLShaderStorageBufferUPtr(new GLShaderStorageBuffer());
        if (!buffer->Init(byteSize, flags)) return nullptr;
        return buffer;
    }

    bool GLShaderStorageBuffer::Init(usize byteSize, uint32 flags)
    {
        glNamedBufferStorage(m_buffer, byteSize, nullptr, flags);
        return true;
    }

    void GLShaderStorageBuffer::Bind() const 
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer); 
    }

    void GLShaderStorageBuffer::BindBase(uint32 slot) const 
    { 
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_buffer); 
    }

    void GLShaderStorageBuffer::Unbind() const 
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 
    }
}