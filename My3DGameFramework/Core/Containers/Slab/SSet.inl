#pragma once

namespace MGF3D
{
    template <typename K, typename Hash, typename KeyEqual>
    usize SSet<K, Hash, KeyEqual>::MemoryUsage() const
    {
        // 컨테이너 객체 + 버킷 테이블 + 노드 오버헤드 합산
        // 일반적인 unordered_set 노드는 데이터(K)와 포인터 2개를 가집니다.
        usize usage = sizeof(*this);
        usage += this->bucket_count() * sizeof(void*);
        usage += this->size() * (sizeof(K) + (sizeof(void*) * 2));
        return usage;
    }

    template <typename K, typename Hash, typename KeyEqual>
    const K* SSet<K, Hash, KeyEqual>::Find(const K& key) const
    {
        auto it = this->find(key);
        if (it != this->end()) return &(*it);
        return nullptr;
    }

    template <typename K, typename Hash, typename KeyEqual>
    bool SSet<K, Hash, KeyEqual>::Insert(const K& key)
    {
        auto result = this->insert(key);
        return result.second;
    }

    template <typename K, typename Hash, typename KeyEqual>
    bool SSet<K, Hash, KeyEqual>::Remove(const K& key)
    {
        return this->erase(key) > 0;
    }
}