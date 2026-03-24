#include "MGFAsyncResult.h"
#pragma once

namespace MGF3D
{
    template<typename T> inline MGFAsyncResult<T>::MGFAsyncResult() = default;
    template<typename T> inline MGF3D::MGFAsyncResult<T>::~MGFAsyncResult() = default;

    template<typename T>
    inline void MGFAsyncResult<T>::Set(T&& data)
    {
        m_data = std::move(data);
        m_isReady.store(true, std::memory_order_release);
    }

    template<typename T>
    inline void MGFAsyncResult<T>::Set(Nullable<T>&& data)
    {
        m_data = std::move(data);
        m_isReady.store(true, std::memory_order_release);
    }

    template<typename T>
    inline bool MGFAsyncResult<T>::IsReady() const
    {
        return m_isReady.load(std::memory_order_acquire);
    }

    template<typename T>
    inline Nullable<T>& MGFAsyncResult<T>::Get()
    {
        return m_data;
    }
}