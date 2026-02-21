#pragma once

namespace MGF3D
{
    inline constexpr uint32 StringHash::Calculate(cstr str, usize len)
    {
        uint32 hash = OFFSET_BASIS;

        for (usize i = 0; i < len; ++i)
        {
            hash ^= static_cast<uint32>(str[i]);
            hash *= PRIME;
        }

        return hash;
    }

    // 문자열 리터럴 생성자
    inline constexpr StringHash::StringHash(cstr s)
        : IHashFunctor<StringHash>
        (
            (s == nullptr) ? 0 : Calculate(s, std::char_traits<char>::length(s))
        ) { }

    // strview 생성자
    inline StringHash::StringHash(strview sv)
        : IHashFunctor<StringHash>(Calculate(sv.data(), sv.size())) { }
}
