#pragma once

CLASS_PTR(InstanceBuffer)
class InstanceBuffer
{
public:
    ~InstanceBuffer();

    static InstanceBufferUPtr Create(uint32 maxCount, uint32 structSize);
    void Update(const void* data, uint32 size);
    void Bind(uint32 slot) const;

private:
    InstanceBuffer();
    bool Init(uint32 maxCount, uint32 structSize);

    uint32 m_id{ 0 };
    uint32 m_maxCount{ 0 };
    uint32 m_structSize{ 0 };
};