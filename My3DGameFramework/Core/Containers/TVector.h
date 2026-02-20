#pragma once
#include <vector>
#include <algorithm>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename T, typename Alloc>
    class TVector : public std::vector<T, Alloc>, public IContainer
    {
    public:
        using Base = std::vector<T, Alloc>;
        using Base::vector;

        TVector() = default;
        virtual ~TVector() = default;

    public:
        usize MemoryUsage() const override { return sizeof(*this) + (this->capacity() * sizeof(T)); }
        usize Count()   const override { return this->size(); }
        bool  Empty()   const override { return this->empty(); }
        void  Clear()         override { this->clear(); }

    public:
        T* Data() { return this->data(); }
        const T* Data() const { return this->data(); }

        void Assign(usize n, const T& value);
        void Reserve(usize n);
        bool Contains(const T& value) const;
        void RemoveSwap(usize index);

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

#include "Containers/TVector.inl"