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
        using Base::operator=;

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
        void  Release() override;

    public:
        Ptr<V> Find(const K& key);
        Ptr<const V> Find(const K& key) const;

        template <typename ArgK, typename ArgV>
        bool Insert(ArgK&& key, ArgV&& value);

        bool Remove(const K& key);
    };
}

#include "Containers/TMap.inl"