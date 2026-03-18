#include "GraphicsPch.h"
#include "VRAMAllocation.h"

namespace MGF3D
{
    VRAMAllocation::VRAMAllocation() noexcept = default;
    VRAMAllocation::VRAMAllocation
    (
        uint64 offset, 
        uint64 size, 
        uint32 bufferHandle, 
        PoolType type
    ) noexcept
        : m_offset(offset), 
          m_size(size), 
          m_bufferHandle(bufferHandle), 
          m_poolType(type) { }

    VRAMAllocation::~VRAMAllocation() noexcept = default;

    bool VRAMAllocation::IsValid() const noexcept
    {
        return m_offset != static_cast<uint64>(-1);
    }

    void VRAMAllocation::Invalidate() noexcept
    {
        m_offset = static_cast<uint64>(-1);
        m_size = 0;
        m_bufferHandle = 0;
    }

    uint64 VRAMAllocation::GetOffset() const noexcept
    {
        return m_offset;
    }

    uint64 VRAMAllocation::GetSize() const noexcept
    {
        return m_size;
    }

    uint32 VRAMAllocation::GetBufferHandle() const noexcept
    {
        return m_bufferHandle;
    }

    VRAMAllocation::PoolType VRAMAllocation::GetPoolType() const noexcept
    {
        return m_poolType;
    }
}