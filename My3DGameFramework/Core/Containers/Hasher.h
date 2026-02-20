#pragma once

namespace MGF3D
{
    template <typename T>
    struct Hasher
    {
        usize operator()(const T& value) const
        {
            return std::hash<T>{}(value);
        }
    };
}