#pragma once

namespace MGF3D
{
    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>::TVector(const TVector<T, OtherAlloc>& other)
        : Base(other.begin(), other.end(), Alloc()) { }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>::TVector(TVector<T, OtherAlloc>&& other) noexcept
        : Base(Alloc())
    {
        this->reserve(other.size());
        this->assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        other.clear();
    }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>& TVector<T, Alloc>::operator=(const TVector<T, OtherAlloc>& other)
    {
        this->assign(other.begin(), other.end());
        return *this;
    }

    template<typename T, typename Alloc>
    template<typename OtherAlloc>
    inline TVector<T, Alloc>& TVector<T, Alloc>::operator=(TVector<T, OtherAlloc>&& other) noexcept
    {
        this->clear();
        this->reserve(other.size());
        this->assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        other.clear();
        return *this;
    }

    template<typename T, typename Alloc>
    inline usize TVector<T, Alloc>::Capacity() const
    {
        return this->capacity();
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Assign(usize n, const T& value)
    {
        this->assign(n, value);
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::Reserve(usize n)
    {
        this->reserve(n);
    }

    template<typename T, typename Alloc>
    inline bool TVector<T, Alloc>::Contains(const T& value) const
    {
        return std::find(this->begin(), this->end(), value) != this->end();
    }

    template<typename T, typename Alloc>
    inline void TVector<T, Alloc>::RemoveSwap(usize index)
    {
        if (index < this->size())
        {
            if (index != this->size() - 1) (*this)[index] = std::move(this->back());
            this->pop_back();
        }
    }
}