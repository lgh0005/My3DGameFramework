#pragma once

CLASS_PTR(Buffer)
class Buffer
{
public:
    static BufferUPtr CreateWithData(uint32 bufferType, uint32 usage, const void* data, usize dataSize);
    ~Buffer();
    uint32 Get() const { return m_buffer; }
    void Bind() const;

private:
    Buffer() {}
    bool Init(uint32 bufferType, uint32 usage, const void* data, usize dataSize);
    uint32 m_buffer     { 0 };
    uint32 m_bufferType { 0 };
    uint32 m_usage      { 0 };
};