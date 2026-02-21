#pragma once

namespace MGF3D
{
    class MemoryUtils
    {
        DECLARE_UTILITIES(MemoryUtils)

    public:
        static void* AlignUp(void* ptr, usize alignment) noexcept;

        static constexpr uintptr AlignUp(uintptr address, usize alignment) noexcept;
        static constexpr usize GetAlignment(usize alignment) noexcept;
        static constexpr uint32 MostSignificantBit64(uint64 value) noexcept;
    };
}

#include "Utils/MemoryUtils.inl"