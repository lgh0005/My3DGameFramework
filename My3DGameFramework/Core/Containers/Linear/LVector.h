#pragma once
#include "Containers/TVector.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
    template<typename T>
    using LVector = TVector<T, LinearAllocator<T>>;
}