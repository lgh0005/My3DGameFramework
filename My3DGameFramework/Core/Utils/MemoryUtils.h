#pragma once

namespace MGF3D
{
    struct MemoryHeader
    {
        Ptr<void> rawPtr; // MemoryManager가 실제로 준 원본 주소
        usize     size;   // MemoryManager에게 반환할 때 필요한 원본 크기
    };

    class MemoryUtils
    {
        MGF_DECLARE_UTILITIES(MemoryUtils)

    public:
        template<typename T> static void MemZero(T& value) noexcept;
        static void Memset(void* dest, int32 value, usize size) noexcept;
        static void Memcpy(void* dest, const void* src, usize size) noexcept;

    public:
        static void* AlignUp(void* ptr, usize alignment) noexcept;
        static constexpr uintptr AlignUp(uintptr address, usize alignment) noexcept;
        static constexpr usize GetAlignment(usize alignment) noexcept;
        static constexpr uint32 MostSignificantBit64(uint64 value) noexcept;

    public:
        template<typename T = void> static T* PtrAdd(void* ptr, usize offset) noexcept;
        template<typename T = void> static T* PtrSub(void* ptr, usize offset) noexcept;
        static constexpr usize GetRequiredSizeWithHeader(usize requestedSize, usize alignment = 16) noexcept;
        static void* PackHeader(void* rawPtr, usize totalSize, usize alignment = 16) noexcept;
        static Ptr<MemoryHeader> UnpackHeader(void* userPtr) noexcept;
    };
}

#include "Utils/MemoryUtils.inl"