#pragma once

namespace MGF3D
{
    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TMap<K, V, Alloc, Hash, KeyEqual>::TMap(const TMap<K, V, OtherAlloc, Hash, KeyEqual>& other)
        : m_map(other.begin(), other.end()) { }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TMap<K, V, Alloc, Hash, KeyEqual>::TMap(TMap<K, V, OtherAlloc, Hash, KeyEqual>&& other) noexcept
        : m_map(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) { }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TMap<K, V, Alloc, Hash, KeyEqual>& TMap<K, V, Alloc, Hash, KeyEqual>::operator=(const TMap<K, V, OtherAlloc, Hash, KeyEqual>& other)
    {
        m_map.clear();
        m_map.insert(other.begin(), other.end());
        return *this;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    template <typename OtherAlloc>
    inline TMap<K, V, Alloc, Hash, KeyEqual>& TMap<K, V, Alloc, Hash, KeyEqual>::operator=(TMap<K, V, OtherAlloc, Hash, KeyEqual>&& other) noexcept
    {
        m_map.clear();
        m_map.insert(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        return *this;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline usize TMap<K, V, Alloc, Hash, KeyEqual>::MemoryUsage() const
    {
        usize bucketMemory = m_map.bucket_count() * sizeof(void*);
        usize nodeMemory = m_map.size() * sizeof(std::pair<const K, V>);
        return sizeof(*this) + bucketMemory + nodeMemory;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline void TMap<K, V, Alloc, Hash, KeyEqual>::Release()
    {
        m_map.clear();
        m_map.rehash(0);
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline Ptr<V> TMap<K, V, Alloc, Hash, KeyEqual>::Find(const K& key)
    {
        auto it = m_map.find(key);
        if (it != m_map.end()) return &it->second;
        return nullptr;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline Ptr<const V> TMap<K, V, Alloc, Hash, KeyEqual>::Find(const K& key) const
    {
        auto it = m_map.find(key);
        if (it != m_map.end()) return &it->second;
        return nullptr;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    template<typename ArgK, typename ArgV>
    inline bool TMap<K, V, Alloc, Hash, KeyEqual>::Insert(ArgK&& key, ArgV&& value)
    {
        auto result = m_map.emplace(std::forward<ArgK>(key), std::forward<ArgV>(value));
        return result.second;
    }

    template<typename K, typename V, typename Alloc, typename Hash, typename KeyEqual>
    inline bool TMap<K, V, Alloc, Hash, KeyEqual>::Remove(const K& key)
    {
        return m_map.erase(key) > 0;
    }
}