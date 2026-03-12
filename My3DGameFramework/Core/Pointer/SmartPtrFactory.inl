#pragma once
#include <utility>
#include "Pointer/SmartPtrAliases.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUnique(Args&&... args)
    {
        void* allocMem = MemoryManager::Instance().Allocate(sizeof(T));
        return UniquePtr<T>(new (allocMem) T(std::forward<Args>(args)...), MGFSmartPtrDeleter<T>());
    }

    template <typename T, typename... Args>
    inline SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::allocate_shared<T>(MGFSmartPtrAllocator<T>(), std::forward<Args>(args)...);
    }
}