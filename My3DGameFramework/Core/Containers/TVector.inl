#pragma once

namespace MGF3D
{
    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>::TVector(const TVector<T, OtherAlloc>& other)
        : m_vector(other.begin(), other.end()) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>::TVector(TVector<T, OtherAlloc>&& other) noexcept
        : m_vector(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) { }

    template<typename T, typename Alloc>
    inline TVector<T, Alloc>::TVector(std::initializer_list<T> init) : m_vector(init) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>& TVector<T, Alloc>::operator=(const TVector<T, OtherAlloc>& other)
    {
        m_vector.assign(other.begin(), other.end()); 
        return *this;
    }

    template<typename T, typename Alloc>
    inline TVector<T, Alloc>& TVector<T, Alloc>::operator=(std::initializer_list<T> init)
    {
        m_vector = init;
        return *this;
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Release()
    {
        m_vector.clear();
        m_vector.shrink_to_fit();
    }

    template<typename T, typename Alloc>
    inline T TVector<T, Alloc>::PopBack()
    {
        MGF_ASSERT(!m_vector.empty(), "TVector is empty. Cannot PopBack!");
        T val = std::move(m_vector.back());
        m_vector.pop_back();
        return val;
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::PushBack(const T& value)
    {
        m_vector.push_back(value);
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::PushBack(T&& value)
    {
        m_vector.push_back(std::move(value));
    }

    template<typename T, typename Alloc>
    template<typename... Args>
    inline void TVector<T, Alloc>::EmplaceBack(Args&&... args)
    {
        m_vector.emplace_back(std::forward<Args>(args)...);
    }

    template<typename T, typename Alloc>
    inline usize TVector<T, Alloc>::Capacity() const
    {
        return m_vector.capacity();
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Assign(usize n, const T& value)
    {
        m_vector.assign(n, value);
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Reserve(usize n)
    {
        m_vector.reserve(n);
    }

    template<typename T, typename Alloc>
    inline bool TVector<T, Alloc>::Contains(const T& value) const
    {
        return std::find(m_vector.begin(), m_vector.end(), value) != m_vector.end();
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::RemoveSwap(usize index)
    {
        if (index < m_vector.size())
        {
            m_vector[index] = std::move(m_vector.back());
            m_vector.pop_back();
        }
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Swap(TVector<T, Alloc>& other) noexcept
    {
        m_vector.swap(other.m_vector);
    }
}