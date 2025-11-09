#include "EnginePch.h"
#include "Buffer.h"

BufferUPtr Buffer::CreateWithData(uint32 bufferType, uint32 usage, 
                        const void* data, usize stride, usize count)
{
    auto buffer = BufferUPtr(new Buffer());
    if (!buffer->Init(bufferType, usage, data, stride, count)) return nullptr;
    return std::move(buffer);
}

Buffer::~Buffer()
{
    if (m_buffer) {
        glDeleteBuffers(1, &m_buffer);
    }
}

void Buffer::Bind() const
{
    glBindBuffer(m_bufferType, m_buffer);
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