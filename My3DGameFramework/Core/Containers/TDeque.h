#pragma once
#include <deque>
#include "Containers/IContainer.h"

namespace MGF3D
{
    template<typename T, typename Alloc>
    class TDeque : public std::deque<T, Alloc>, public IContainer
    {
    public:
        using Base = std::deque<T, Alloc>;
        using Base::deque;
        using Base::operator=;

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
        usize Count()       const override { return this->size(); }
        bool  Empty()       const override { return this->empty(); }
        void  Clear()             override { this->clear(); }

    public:
        void PushBack(T&& value);
        void PushBack(const T& value);
        void PushFront(T&& value);
        void PushFront(const T& value);

        T PopFront();
        T PopBack();

        T& Front() { return this->front(); }
        const T& Front() const { return this->front(); }
        T& Back() { return this->back(); }
        const T& Back() const { return this->back(); }
    };
}

#include "Containers/TDeque.inl"