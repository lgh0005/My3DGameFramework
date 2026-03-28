#pragma once
#include <vector>
#include <algorithm>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename T, typename Alloc>
    class TVector : public IContainer
    {
    private:
        std::vector<T, Alloc> m_vector;

    public:
        TVector() = default;
        virtual ~TVector() = default;
        TVector(std::initializer_list<T> init);

    public:
        // 1. 다른 할당자를 사용하는 TVector로부터 복사 생성
        template<typename OtherAlloc>
        TVector(const TVector<T, OtherAlloc>& other);

        // 2. 다른 할당자를 사용하는 TVector로부터 이동 생성
        template<typename OtherAlloc>
        TVector(TVector<T, OtherAlloc>&& other) noexcept;

        // 3. 대입 연산자
        template<typename OtherAlloc>
        TVector& operator=(const TVector<T, OtherAlloc>& other);
        TVector& operator=(std::initializer_list<T> init);

    public:
        usize MemoryUsage() const override { return sizeof(*this) + (m_vector.capacity() * sizeof(T)); }
        usize Count()   const override { return m_vector.size(); }
        bool  Empty()   const override { return m_vector.empty(); }
        void  Clear()         override { m_vector.clear(); }
        void  Release();

    public:
        template<typename U = T, std::enable_if_t<!std::is_same_v<U, bool>, int> = 0>
        Ptr<T> Data() { return m_vector.data(); }

        template<typename U = T, std::enable_if_t<!std::is_same_v<U, bool>, int> = 0>
        Ptr<const T> Data() const { return m_vector.data(); }

    public:
        T PopBack();

        template<typename... Args> void EmplaceBack(Args&&... args);
        void PushBack(const T& value);
        void PushBack(T&& value);

        void Resize(usize n) { m_vector.resize(n); }
        void Resize(usize n, const T& value) { m_vector.resize(n, value); }

        usize Capacity() const;
        void Assign(usize n, const T& value);
        void Reserve(usize n);
        bool Contains(const T& value) const;
        void RemoveSwap(usize index);
        void Swap(TVector<T, Alloc>& other) noexcept;

    public:
        using iterator = typename std::vector<T, Alloc>::iterator;
        using const_iterator = typename std::vector<T, Alloc>::const_iterator;

        iterator begin() { return m_vector.begin(); }
        iterator end() { return m_vector.end(); }
        const_iterator begin() const { return m_vector.begin(); }
        const_iterator end()   const { return m_vector.end(); }

        using reference = typename std::vector<T, Alloc>::reference;
        using const_reference = typename std::vector<T, Alloc>::const_reference;
        reference operator[](usize index) { return m_vector[index]; }
        const_reference operator[](usize index) const { return m_vector[index]; }

        reference Back() { return m_vector.back(); }
        const_reference Back() const { return m_vector.back(); }
    };
}

#include "Containers/TVector.inl"