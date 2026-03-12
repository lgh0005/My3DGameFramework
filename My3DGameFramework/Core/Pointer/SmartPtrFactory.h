#pragma once
#include "Pointer/SmartPtrAliases.h"

namespace MGF3D
{
    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUnique(Args&&... args);

    template <typename T, typename... Args>
    inline SharedPtr<T> MakeShared(Args&&... args);
}

#include "Pointer/SmartPtrFactory.inl"