#pragma once
#include <unordered_set>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template <typename K, typename Alloc, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    class TSet : public std::unordered_set<K, Hash, KeyEqual, Alloc>, public IContainer
    {
    private:
        std::unordered_set<K, Hash, KeyEqual, Alloc> m_set;

    public:
        TSet() = default;
        virtual ~TSet() = default;

    public:
        template <typename OtherAlloc>
        TSet(const TSet<K, OtherAlloc, Hash, KeyEqual>& other);

        template <typename OtherAlloc>
        TSet(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept;

        template <typename OtherAlloc>
        TSet& operator=(const TSet<K, OtherAlloc, Hash, KeyEqual>& other);

        template <typename OtherAlloc>
        TSet& operator=(TSet<K, OtherAlloc, Hash, KeyEqual>&& other) noexcept;

    public:
        usize MemoryUsage() const override;
        usize Count()       const override { return m_set.size(); }
        bool  Empty()       const override { return m_set.empty(); }
        void  Clear()             override { m_set.clear(); }
        void  Release()           override;

    public:
        Ptr<const K> Find(const K& key) const;
        bool Insert(const K& key);
        bool Remove(const K& key);

    public:
        using iterator = typename std::unordered_set<K, Hash, KeyEqual, Alloc>::iterator;
        using const_iterator = typename std::unordered_set<K, Hash, KeyEqual, Alloc>::const_iterator;

        iterator begin() { return m_set.begin(); }
        iterator end() { return m_set.end(); }
        const_iterator begin() const { return m_set.begin(); }
        const_iterator end()   const { return m_set.end(); }
    };
}

#include "Containers/TSet.inl"