#pragma once
#include "Containers/TStaticLinkedList.h"
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
    template<typename T>
    using SStaticLinkedList = TStaticLinkedList<T, SlabAllocator<T>>;
}