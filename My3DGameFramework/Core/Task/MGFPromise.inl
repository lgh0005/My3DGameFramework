#pragma once

namespace MGF3D
{
    template<typename T>
    MGFPromise<T>::MGFPromise() = default;

    template<typename T>
    MGFFuture<T> MGFPromise<T>::GetFuture()
    {
        return MGFFuture<T>(m_promise.get_future());
    }

    template<typename T>
    void MGFPromise<T>::SetValue(const T& value)
    {
        m_promise.set_value(value);
    }

    template<typename T>
    void MGFPromise<T>::SetValue(T&& value)
    {
        m_promise.set_value(std::move(value));
    }
}