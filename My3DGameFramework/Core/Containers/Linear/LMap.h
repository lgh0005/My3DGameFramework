#pragma once
#include "Containers/TMap.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
    template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    using LMap = TMap<K, V, LinearAllocator<std::pair<const K, V>>, Hash, KeyEqual>;
}