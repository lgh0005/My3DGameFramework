#pragma once
#include "Managers/MemoryManager.h"
#include "Pointer/SharedPtr.h"
#include "Pointer/UniquePtr.h"

namespace MGF3D
{
    template <typename T, typename... Args>
    inline SharedPtr<T> MakeShared(Args&&... args)
    {
        void* mem = MGF_MEMORY.Allocate(sizeof(T));
        MGF_ASSERT(mem != nullptr, "MakeShared: Allocation failed.");
        T* rawPtr = ::new (mem) T(std::forward<Args>(args)...);
        return SharedPtr<T>(rawPtr);
    }

    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUnique(Args&&... args)
    {
        void* mem = MGF_MEMORY.Allocate(sizeof(T));
        MGF_ASSERT(mem != nullptr, "MakeUnique: Allocation failed.");
        T* rawPtr = ::new (mem) T(std::forward<Args>(args)...);
        return UniquePtr<T>(rawPtr);
    }
}