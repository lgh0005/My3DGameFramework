#pragma once
#include "Managers/MemoryManager.h"
#include "Pointer/SharedPtr.h"
#include "Pointer/UniquePtr.h"
#include "Pointer/RefCount.h"
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
    template <typename T, typename... Args>
    inline SharedPtr<T> MakeShared(Args&&... args)
    {
        MGF_STATIC_ASSERT(std::is_base_of_v<RefCount, T>, "MakeShared: Type T must inherit from RefCount!");
        return SharedPtr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUnique(Args&&... args)
    {
        MGF_STATIC_ASSERT(std::is_base_of_v<PoolAlloc, T>, "MakeUnique: Type T must inherit from PoolAlloc!");
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }
}