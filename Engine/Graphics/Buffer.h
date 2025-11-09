#pragma once

CLASS_PTR(Buffer)
class Buffer
{
public:
    static BufferUPtr CreateWithData(uint32 bufferType, 
        uint32 usage, const void* data, usize stride, usize count);
    
    ~Buffer();
    uint32 Get() const { return m_buffer; }
    size_t GetStride() const { return m_stride; }
    size_t GetCount() const { return m_count; }
    void Bind() const;

private:
    Buffer() {}
    bool Init(uint32 bufferType, uint32 usage, 
        const void* data, usize stride, usize count);
    uint32 m_buffer     { 0 };
    uint32 m_bufferType { 0 };
    uint32 m_usage      { 0 };
    usize  m_stride     { 0 };
    usize  m_count      { 0 };
};