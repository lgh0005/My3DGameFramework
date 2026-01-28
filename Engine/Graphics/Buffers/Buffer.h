#pragma once

CLASS_PTR(Buffer)
class Buffer
{
public:
    static BufferUPtr CreateWithData(uint32 bufferType, 
        uint32 usage, const void* data, usize stride, usize count);
    
public:
    virtual ~Buffer();
    uint32 Get() const { return m_buffer; }
    usize GetStride() const { return m_stride; }
    usize GetCount() const { return m_count; }

    void SetData(const void* data, usize size);
    void Bind() const;
    void BindBase(uint32 index) const;

protected:
    Buffer();
    bool Init(uint32 bufferType, uint32 usage, 
        const void* data, usize stride, usize count);
    uint32 m_buffer     { 0 };
    uint32 m_bufferType { 0 };
    uint32 m_usage      { 0 };
    usize  m_stride     { 0 };
    usize  m_count      { 0 };
};