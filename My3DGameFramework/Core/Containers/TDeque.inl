#pragma once

namespace MGF3D
{
    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>::TDeque(const TDeque<T, OtherAlloc>& other)
        : Base(other.begin(), other.end(), Alloc()) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>::TDeque(TDeque<T, OtherAlloc>&& other) noexcept
        : Base(Alloc())
    {
        // deque는 reserve()가 없으므로 바로 assign을 수행
        this->assign
        (
            std::make_move_iterator(other.begin()), 
            std::make_move_iterator(other.end())
        );
        other.clear();
    }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>& TDeque<T, Alloc>::operator=(const TDeque<T, OtherAlloc>& other)
    {
        this->assign(other.begin(), other.end());
        return *this;
    }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>& TDeque<T, Alloc>::operator=(TDeque<T, OtherAlloc>&& other) noexcept
    {
        this->clear();
        this->assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        other.clear();
        return *this;
    }

    template<typename T, typename Alloc>
    inline usize TDeque<T, Alloc>::MemoryUsage() const
    {
        // 실제 요소들의 크기 합을 반환
        return sizeof(*this) + (this->size() * sizeof(T));
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushBack(T&& value)
    {
        this->push_back(std::forward<T>(value));
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushBack(const T& value)
    {
        this->push_back(value);
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushFront(T&& value)
    {
        this->push_front(std::forward<T>(value));
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushFront(const T& value)
    {
        this->push_front(value);
    }

    template<typename T, typename Alloc>
    inline T TDeque<T, Alloc>::PopFront()
    {
        MGF_ASSERT(!this->empty(), "TDeque is empty.");
        T val = std::move(this->front());
        this->pop_front();
        return val;
    }

    template<typename T, typename Alloc>
    inline T TDeque<T, Alloc>::PopBack()
    {
        MGF_ASSERT(!this->empty(), "TDeque is empty.");
        T val = std::move(this->back());
        this->pop_back();
        return val;
    }
}