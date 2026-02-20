#pragma once

namespace MGF3D
{
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