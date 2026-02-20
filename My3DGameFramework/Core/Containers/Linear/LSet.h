#pragma once
#include "Containers/TSet.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
    template<typename K, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    using LSet = TSet<K, LinearAllocator<K>, Hash, KeyEqual>;
}