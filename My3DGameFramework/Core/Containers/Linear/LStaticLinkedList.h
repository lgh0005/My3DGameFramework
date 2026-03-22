#pragma once
#include "Containers/TStaticLinkedList.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
    template<typename T>
    using LStaticLinkedList = TStaticLinkedList<T, LinearAllocator<T>>;
}