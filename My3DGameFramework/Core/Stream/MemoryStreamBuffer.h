#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
    MGF_CLASS_PTR(MemoryStreamBufferPool)

    MGF_CLASS_PTR(MemoryStreamBuffer)
    class MemoryStreamBuffer : public RefCount
    {
    public:
        MemoryStreamBuffer(usize capacity);
        virtual ~MemoryStreamBuffer() override;

    public:
        byte* GetPtr() { return m_data.Data(); }
        const byte* GetPtr() const { return m_data.Data(); }
        usize GetCapacity() const { return m_data.Capacity(); }
        usize GetSize() const { return m_data.Count(); }

    private:
        SVector<byte> m_data;
    };
}