#pragma once

namespace MGF3D
{
    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>::TSet(const TSet<K, OtherAlloc, Hash, KeyEqual>& other)
        : m_set(other.begin(), other.end()) { }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>::TSet(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept
        : m_set(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) { }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>& TSet<K, Alloc, Hash, KeyEqual>::operator=(const TSet<K, OtherAlloc, Hash, KeyEqual>& other)
    {
        m_set.clear();
        m_set.insert(other.begin(), other.end());
        return *this;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TSet<K, Alloc, Hash, KeyEqual>& TSet<K, Alloc, Hash, KeyEqual>::operator=(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept
    {
        m_set.clear();
        m_set.insert(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        return *this;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline usize TSet<K, Alloc, Hash, KeyEqual>::MemoryUsage() const
    {
        usize usage = sizeof(*this);
        usage += m_set.bucket_count() * sizeof(void*);
        usage += m_set.size() * (sizeof(K) + (sizeof(void*) * 2));
        return usage;
    }

    template<typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline void TSet<K, Alloc, Hash, KeyEqual>::Release()
    {
        m_set.clear();
        m_set.rehash(0);
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline Ptr<const K> TSet<K, Alloc, Hash, KeyEqual>::Find(const K& key) const
    {
        auto it = m_set.find(key);
        if (it != m_set.end()) return &(*it);
        return nullptr;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TSet<K, Alloc, Hash, KeyEqual>::Insert(const K& key)
    {
        auto result = m_set.insert(key);
        return result.second;
    }

    template <typename K, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TSet<K, Alloc, Hash, KeyEqual>::Remove(const K& key)
    {
        return m_set.erase(key) > 0;
    }
}