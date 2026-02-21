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
        // 1. 다른 할당자를 사용하는 TVector로부터 복사 생성
        template<typename OtherAlloc>
        TVector(const TVector<T, OtherAlloc>& other);

        // 2. 다른 할당자를 사용하는 TVector로부터 이동 생성 (Virtual Move)
        template<typename OtherAlloc>
        TVector(TVector<T, OtherAlloc>&& other) noexcept;

        // 3. 다른 할당자로부터의 대입 연산자
        template<typename OtherAlloc>
        TVector& operator=(const TVector<T, OtherAlloc>& other);

        template<typename OtherAlloc>
        TVector& operator=(TVector<T, OtherAlloc>&& other) noexcept;

    public:
        usize MemoryUsage() const override { return sizeof(*this) + (this->capacity() * sizeof(T)); }
        usize Count()   const override { return this->size(); }
        bool  Empty()   const override { return this->empty(); }
        void  Clear()         override { this->clear(); }

    public:
        T* Data() { return this->data(); }
        const T* Data() const { return this->data(); }

        usize Capacity() const;
        void Assign(usize n, const T& value);
        void Reserve(usize n);
        bool Contains(const T& value) const;
        void RemoveSwap(usize index);

    public:
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;
        using reverse_iterator = typename Base::reverse_iterator;
        using const_reverse_iterator = typename Base::const_reverse_iterator;

        iterator       begin()        noexcept { return Base::begin(); }
        iterator       end()          noexcept { return Base::end(); }
        const_iterator begin()  const noexcept { return Base::begin(); }
        const_iterator end()    const noexcept { return Base::end(); }
        const_iterator cbegin() const noexcept { return Base::cbegin(); }
        const_iterator cend()   const noexcept { return Base::cend(); }

        reverse_iterator       rbegin()        noexcept { return Base::rbegin(); }
        reverse_iterator       rend()          noexcept { return Base::rend(); }
        const_reverse_iterator crbegin() const noexcept { return Base::crbegin(); }
        const_reverse_iterator crend()   const noexcept { return Base::crend(); }
    };
}

#include "Containers/TVector.inl"