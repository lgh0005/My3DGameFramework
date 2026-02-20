#pragma once
#include "Containers/TMap.h"
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
    // 영구적인 매핑 데이터 관리용 Slab Map
    template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    using SMap = TMap<K, V, SlabAllocator<std::pair<const K, V>>, Hash, KeyEqual>;
}