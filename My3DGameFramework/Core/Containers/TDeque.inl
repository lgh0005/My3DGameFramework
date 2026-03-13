#pragma once

namespace MGF3D
{
    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>::TDeque(const TDeque<T, OtherAlloc>& other)
        : m_deque(other.begin(), other.end()) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>::TDeque(TDeque<T, OtherAlloc>&& other) noexcept
        : m_deque(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>& TDeque<T, Alloc>::operator=(const TDeque<T, OtherAlloc>& other)
    {
        m_deque.assign(other.begin(), other.end());
        return *this;
    }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TDeque<T, Alloc>& TDeque<T, Alloc>::operator=(TDeque<T, OtherAlloc>&& other) noexcept
    {
        m_deque.assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        return *this;
    }

    template<typename T, typename Alloc>
    inline usize TDeque<T, Alloc>::MemoryUsage() const
    {
        return sizeof(*this) + (m_deque.size() * sizeof(T));
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::Release()
    {
        m_deque.clear();
        m_deque.shrink_to_fit();
    }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushBack(T&& value) { m_deque.push_back(std::move(value)); }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushBack(const T& value) { m_deque.push_back(value); }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushFront(T&& value) { m_deque.push_front(std::move(value)); }

    template<typename T, typename Alloc>
    inline void TDeque<T, Alloc>::PushFront(const T& value) { m_deque.push_front(value); }

    template<typename T, typename Alloc>
    inline T TDeque<T, Alloc>::PopFront()
    {
        MGF_ASSERT(!m_deque.empty(), "TDeque is empty.");
        T val = std::move(m_deque.front());
        m_deque.pop_front();
        return val;
    }

    template<typename T, typename Alloc>
    inline T TDeque<T, Alloc>::PopBack()
    {
        MGF_ASSERT(!m_deque.empty(), "TDeque is empty.");
        T val = std::move(m_deque.back());
        m_deque.pop_back();
        return val;
    }
}