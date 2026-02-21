#pragma once
#include "Utils/CoreUtils.h"

namespace MGF3D
{
    inline constexpr uint32 StringHash::Calculate(cstr str, usize len)
    {
        uint32 hash = OFFSET_BASIS;

        for (usize i = 0; i < len; ++i)
        {
            char8 c = str[i];
            if (CoreUtils::IsBetween<char8>(c, 'A', 'Z'))
                c += 32;

            hash ^= static_cast<uint32>(c);
            hash *= PRIME;
        }

        return hash;
    }

    // 문자열 리터럴 생성자
    inline constexpr StringHash::StringHash(cstr s)
        : IHashFunctor<StringHash>
        ((s == nullptr) ? 0 : Calculate(s, std::char_traits<char>::length(s))) { }

    // strview 생성자
    inline StringHash::StringHash(strview sv)
        : IHashFunctor<StringHash>(Calculate(sv.data(), sv.size())) { }
}
