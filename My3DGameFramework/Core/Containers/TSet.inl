#pragma once

namespace MGF3D
{
    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>::TSet(const TSet<K, OtherAlloc, Hash, KeyEqual>& other)
        : Base(other.begin(), other.end(), 0, Hash(), KeyEqual(), Alloc()) { }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>::TSet(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept
        : Base(Alloc())
    {
        this->reserve(other.size());
        this->insert(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        other.clear();
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>& TSet<K, Alloc, Hash, KeyEqual>::operator=(const TSet<K, OtherAlloc, Hash, KeyEqual>& other)
    {
        this->clear();
        this->insert(other.begin(), other.end());
        return *this;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>& TSet<K, Alloc, Hash, KeyEqual>::operator=(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept
    {
        this->clear();
        this->reserve(other.size());
        this->insert(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        other.clear();
        return *this;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline usize TSet<K, Alloc, Hash, KeyEqual>::MemoryUsage() const
    {
        // 컨테이너 객체 + 버킷 테이블 + 노드 오버헤드 합산
        // std::unordered_set의 노드 구조: 데이터(K) + 포인터 2개(next, prev 등)
        usize usage = sizeof(*this);
        usage += this->bucket_count() * sizeof(void*);
        usage += this->size() * (sizeof(K) + (sizeof(void*) * 2));
        return usage;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline const K* TSet<K, Alloc, Hash, KeyEqual>::Find(const K& key) const
    {
        auto it = this->find(key);
        if (it != this->end()) return &(*it);
        return nullptr;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TSet<K, Alloc, Hash, KeyEqual>::Insert(const K& key)
    {
        auto result = this->insert(key);
        return result.second;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TSet<K, Alloc, Hash, KeyEqual>::Remove(const K& key)
    {
        return this->erase(key) > 0;
    }
}