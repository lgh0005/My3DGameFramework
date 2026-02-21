#pragma once
#include <charconv>
#include "Containers/Linear/LString.h"

namespace MGF3D
{
    class StringUtils
    {
        DECLARE_UTILITIES(StringUtils)

    public:
        template<typename T> static LString ToString(T value);

        static const LString& Empty();

        template<typename Alloc>
        static bool IsNullOrEmpty(const TString<Alloc>& s);
        static bool IsNullOrEmpty(cstr s);
        static bool IsNullOrEmpty(strview sv);
        static bool IsNullOrEmpty(const str& s);

        static constexpr char8 ToLower(char8 c);
        static constexpr char8 ToUpper(char8 c);

        static int32 Compare(cstr s1, cstr s2);
        static int32 Compare(strview s1, strview s2);

        static bool Equals(strview s1, strview s2);
    };
}

#include "Utils/StringUtils.inl"