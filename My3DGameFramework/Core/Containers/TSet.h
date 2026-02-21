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
        // IContainer 구현
        usize MemoryUsage() const override;
        usize Count()   const override { return this->size(); }
        bool  Empty()   const override { return this->empty(); }
        void  Clear()         override { this->clear(); }

    public:
        // Set의 특성상 내부 데이터 수정을 방지하기 위해 Read-only Find만 제공
        const K* Find(const K& key) const;
        bool Insert(const K& key);
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

#include "Containers/TSet.inl"