#pragma once

namespace MGF3D
{
    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline usize TMap<K, V, Alloc, Hash, KeyEqual>::MemoryUsage() const
    {
        // 컨테이너 자체 + 버킷 포인터 배열 + 노드 오버헤드
        // std::pair<const K, V>는 실제 노드에 저장되는 데이터 타입입니다.
        usize usage = sizeof(*this);
        usage += this->bucket_count() * sizeof(void*);
        usage += this->size() * (sizeof(std::pair<const K, V>) + (sizeof(void*) * 2));
        return usage;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline V* TMap<K, V, Alloc, Hash, KeyEqual>::Find(const K& key)
    {
        auto it = this->find(key);
        if (it != this->end()) return &it->second;
        return nullptr;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline const V* TMap<K, V, Alloc, Hash, KeyEqual>::Find(const K& key) const
    {
        auto it = this->find(key);
        if (it != this->end()) return &it->second;
        return nullptr;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TMap<K, V, Alloc, Hash, KeyEqual>::Insert(const K& key, const V& value)
    {
        // emplace를 사용하여 복사 비용 최적화
        auto result = this->emplace(key, value);
        return result.second;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TMap<K, V, Alloc, Hash, KeyEqual>::Remove(const K& key)
    {
        return this->erase(key) > 0;
    }
}