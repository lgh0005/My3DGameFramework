#pragma once
#include <unordered_map>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename K, typename V, typename Alloc, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    class TMap : public std::unordered_map<K, V, Hash, KeyEqual, Alloc>, public IContainer
    {
    public:
        using Base = std::unordered_map<K, V, Hash, KeyEqual, Alloc>;
        using Base::unordered_map;

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
        usize Count()   const override { return this->size(); }
        bool  Empty()   const override { return this->empty(); }
        void  Clear()         override { this->clear(); }

    public:
        V* Find(const K& key);
        const V* Find(const K& key) const;
        bool Insert(const K& key, const V& value);
        bool Remove(const K& key);

    public:
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        iterator       begin()        noexcept { return Base::begin(); }
        iterator       end()          noexcept { return Base::end(); }
        const_iterator begin()  const noexcept { return Base::begin(); }
        const_iterator end()    const noexcept { return Base::end(); }
        const_iterator cbegin() const noexcept { return Base::cbegin(); }
        const_iterator cend()   const noexcept { return Base::cend(); }
    };
}

#include "Containers/TMap.inl"