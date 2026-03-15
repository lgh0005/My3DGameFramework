#pragma once
#include <unordered_map>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename K, typename V, typename Alloc, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    class TMap : public IContainer
    {
    private:
        std::unordered_map<K, V, Hash, KeyEqual, Alloc> m_map;

    public:
        TMap() = default;
        virtual ~TMap() = default;

    public:
        template <typename OtherAlloc>
        TMap(const TMap<K, V, OtherAlloc, Hash, KeyEqual>& other);

        template <typename OtherAlloc>
        TMap(TMap<K, V, OtherAlloc, Hash, KeyEqual>&& other) noexcept;

        template <typename OtherAlloc>
        TMap& operator=(const TMap<K, V, OtherAlloc, Hash, KeyEqual>& other);

        template <typename OtherAlloc>
        TMap& operator=(TMap<K, V, OtherAlloc, Hash, KeyEqual>&& other) noexcept;

    public:
        usize MemoryUsage() const override;
        usize Count()       const override { return m_map.size(); }
        bool  Empty()       const override { return m_map.empty(); }
        void  Clear()             override { m_map.clear(); }
        void  Release()           override;

    public:
        Ptr<V> Find(const K& key);
        Ptr<const V> Find(const K& key) const;
        bool Contains(const K& key) const { return m_map.find(key) != m_map.end(); }

        template <typename ArgK, typename ArgV>
        bool Insert(ArgK&& key, ArgV&& value);

        bool Remove(const K& key);

    public:
        using iterator = typename std::unordered_map<K, V, Hash, KeyEqual, Alloc>::iterator;
        using const_iterator = typename std::unordered_map<K, V, Hash, KeyEqual, Alloc>::const_iterator;

        iterator begin() { return m_map.begin(); }
        iterator end() { return m_map.end(); }
        const_iterator begin() const { return m_map.begin(); }
        const_iterator end()   const { return m_map.end(); }

        V& operator[](const K& key) { return m_map[key]; }
        V& operator[](K&& key) { return m_map[std::move(key)]; }
    };
}

#include "Containers/TMap.inl"