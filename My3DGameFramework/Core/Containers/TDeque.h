#pragma once
#include <deque>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename T, typename Alloc>
    class TDeque : public IContainer
    {
    private:
        std::deque<T, Alloc> m_deque;

    public:
        TDeque() = default;
        virtual ~TDeque() = default;

    public:
        template<typename OtherAlloc>
        TDeque(const TDeque<T, OtherAlloc>& other);

        template<typename OtherAlloc>
        TDeque(TDeque<T, OtherAlloc>&& other) noexcept;

        template<typename OtherAlloc>
        TDeque& operator=(const TDeque<T, OtherAlloc>& other);

        template<typename OtherAlloc>
        TDeque& operator=(TDeque<T, OtherAlloc>&& other) noexcept;

    public:
        usize MemoryUsage() const override;
        usize Count()       const override { return m_deque.size(); }
        bool  Empty()       const override { return m_deque.empty(); }
        void  Clear()             override { m_deque.clear(); }
        void  Release() override;

    public:
        void PushBack(T&& value);
        void PushBack(const T& value);
        void PushFront(T&& value);
        void PushFront(const T& value);

        T PopFront();
        T PopBack();

        T& Front() { return m_deque.front(); }
        const T& Front() const { return m_deque.front(); }
        T& Back() { return m_deque.back(); }
        const T& Back() const { return m_deque.back(); }

    public:
        using iterator = typename std::deque<T, Alloc>::iterator;
        using const_iterator = typename std::deque<T, Alloc>::const_iterator;

        iterator begin() { return m_deque.begin(); }
        iterator end() { return m_deque.end(); }
        const_iterator begin() const { return m_deque.begin(); }
        const_iterator end()   const { return m_deque.end(); }

        T& operator[](usize index) { return m_deque[index]; }
        const T& operator[](usize index) const { return m_deque[index]; }
    };
}

#include "Containers/TDeque.inl"