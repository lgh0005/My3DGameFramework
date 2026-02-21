#pragma once
#include <unordered_set>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template <typename K, typename Alloc, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
    class TSet : public std::unordered_set<K, Hash, KeyEqual, Alloc>, public IContainer
    {
    public:
        using Base = std::unordered_set<K, Hash, KeyEqual, Alloc>;
        using Base::unordered_set;

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
        usize Count()   const override { return this->size(); }
        bool  Empty()   const override { return this->empty(); }
        void  Clear()         override { this->clear(); }

    public:
        // Set의 특성상 내부 데이터 수정을 방지하기 위해 Read-only Find만 제공
        RawPtr<const K> Find(const K& key) const;
        bool Insert(const K& key);
        bool Remove(const K& key);
    };
}

#include "Containers/TSet.inl"