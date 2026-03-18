#pragma once

namespace MGF3D
{
    class VRAMAllocation
    {
    public:
        enum class PoolType : uint8
        {
            Static,  // 업데이트가 거의 없는 정적 데이터
            Dynamic  // 매 프레임 업데이트되는 동적 데이터
        };

    public:
        VRAMAllocation() noexcept;
        VRAMAllocation
        (
            uint64 offset, 
            uint64 size, 
            uint32 bufferHandle, 
            PoolType type
        ) noexcept;
        ~VRAMAllocation() noexcept;

        bool IsValid() const noexcept;
        void Invalidate() noexcept;

    public:
        uint64   GetOffset() const noexcept;
        uint64   GetSize() const noexcept;
        uint32   GetBufferHandle() const noexcept;
        PoolType GetPoolType() const noexcept;

    private:
        uint64   m_offset       { static_cast<uint64>(-1) };
        uint64   m_size         { 0 };
        uint32   m_bufferHandle { 0 };
        PoolType m_poolType     { PoolType::Static };
    };
}