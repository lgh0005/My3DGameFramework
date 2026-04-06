#include "EnginePch.h"
#include "Buffer.h"

Buffer::Buffer() = default;
Buffer::~Buffer()
{
    if (m_buffer) glDeleteBuffers(1, &m_buffer);
}

BufferUPtr Buffer::CreateWithData(uint32 bufferType, uint32 usage, 
                        const void* data, usize stride, usize count)
{
    auto buffer = BufferUPtr(new Buffer());
    if (!buffer->Init(bufferType, usage, data, stride, count)) return nullptr;
    return std::move(buffer);
}

void Buffer::SetData(const void* data, usize size)
{
    Bind();
    glBufferSubData(m_bufferType, 0, size, data);
}

void Buffer::Bind() const
{
    glBindBuffer(m_bufferType, m_buffer);
}

void Buffer::Unbind() const
{
    glBindBuffer(m_bufferType, 0);
}

void Buffer::BindBase(uint32 index) const
{
    glBindBufferBase(m_bufferType, index, m_buffer);
}

bool Buffer::Init(uint32 bufferType, uint32 usage, 
                  const void* data, usize stride, usize count)
{
    m_bufferType = bufferType;
    m_usage = usage;
    m_stride = stride;
    m_count = count;
    glGenBuffers(1, &m_buffer);
    Bind();
    glBufferData(m_bufferType, m_stride * m_count, data, usage);
    return true;
}