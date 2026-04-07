#pragma once
#include "CoreUtils/CommonUtils.h"

namespace MGF3D
{
    inline constexpr StringHash operator""_sh(cstr s, usize n)
    {
        return StringHash{ StringView{ s, n } };
    }

    inline constexpr uint32 StringHash::Calculate(cstr str, usize len)
    {
		if (str == nullptr || len == 0) return 0;

        uint32 hash = OFFSET_BASIS;
        for (usize i = 0; i < len; ++i)
        {
            char c { str[i] };
            if (CommonUtils::IsBetween<char>(c, 'A', 'Z')) c += 32;
            hash ^= static_cast<uint32>(c);
            hash *= PRIME;
        }

        return hash;
    }

    inline constexpr StringHash::StringHash(StringView sv)
        : IHashFunctor<StringHash>{ Calculate(sv.data(), sv.size()) } { }
}