#pragma once
#include <optional>

namespace MGF3D
{
    template <typename T>
    class Nullable
    {
    public:
        Nullable();
        Nullable(none_t);
        Nullable(const T& value);
        Nullable(T&& value);

    public:
        bool IsValid() const { return m_value.has_value(); }
        bool IsNull() const { return !m_value.has_value(); }

    public:
        T& Get() { return m_value.value(); }
        const T& Get() const { return m_value.value(); }
        T GetValueOr(const T& defaultValue) const { return m_value.value_or(defaultValue); }
        void Reset() { m_value.reset(); }

    public:
        T* operator->() { return &m_value.value(); }
        const T* operator->() const { return &m_value.value(); }
        T& operator*() { return m_value.value(); }
        const T& operator*() const { return m_value.value(); }
        explicit operator bool() const { return IsValid(); }
        bool operator==(none_t) const { return IsNull(); }
        bool operator!=(none_t) const { return IsValid(); }

    private:
        std::optional<T> m_value;
    };
}

#include "Aliases/Nullable.inl"