#pragma once

namespace MGF3D
{
    template <typename T>
    struct MGFSmartPtrDeleter
    {
        void operator()(T* ptr) const;
    };

    template <typename T>
    struct MGFSmartPtrAllocator
    {
        using value_type = T;

        template <class U> struct rebind
        {
            using other = MGFSmartPtrAllocator<U>;
        };

        MGFSmartPtrAllocator() noexcept = default;
        template <typename U> MGFSmartPtrAllocator(const MGFSmartPtrAllocator<U>&) noexcept {}

        T* allocate(std::size_t n);
        void deallocate(T* p, std::size_t n);

        template <typename U> bool operator==(const MGFSmartPtrAllocator<U>&) const noexcept { return true; }
        template <typename U> bool operator!=(const MGFSmartPtrAllocator<U>&) const noexcept { return false; }
    };
}

#include "Pointer/SmartPtrCore.inl"