#include "EnginePch.h"
#include "UniformBuffer.h"

Uniformbuffer::Uniformbuffer() = default;
Uniformbuffer::~Uniformbuffer()
{
    if (m_buffer) glDeleteBuffers(1, &m_buffer);
}

UniformbufferUPtr Uniformbuffer::Create(uint32 size, uint32 bindingPoint)
{
    auto uniformbuffer = UniformbufferUPtr(new Uniformbuffer());
    uniformbuffer->Init(size, bindingPoint);
    return std::move(uniformbuffer);
}

void Uniformbuffer::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
}

void Uniformbuffer::Init(uint32 size, uint32 bindingPoint)
{
    m_bindingPoint = bindingPoint;
    m_size = size;

    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    // 버퍼 ID를 지정된 Binding Point에 연결
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Uniformbuffer::SetData(const void* data, uint32 size, uint32 offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}