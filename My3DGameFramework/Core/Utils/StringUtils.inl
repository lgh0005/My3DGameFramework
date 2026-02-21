#pragma once
#include "StringUtils.h"
#include "Debug/Asserter.h"
#include "Utils/CoreUtils.h"

namespace MGF3D
{
    template<typename T>
    inline LString StringUtils::ToString(T value)
    {
        // 1. 컴파일 타임 타입 검사
        MGF_STATIC_ASSERT(std::is_arithmetic_v<T>, "T must be an arithmetic type.");

        // 2. 타입별 최대 버퍼 크기 계산
        // 64비트 정수는 최대 20자, 실수는 넉넉하게 32자면 충분합니다.
        constexpr usize MaxBufferSize = std::is_floating_point_v<T> ? 64 : 24;
        char8 buffer[MaxBufferSize];

        // 3. 변환 수행
        auto [ptr, ec] = std::to_chars(buffer, buffer + MaxBufferSize, value);
        MGF_ASSERT(ec == std::errc(), "Failed to convert arithmetic value to string.");
    
        // 5. 유효 범위만큼 LString 생성하여 반환 (프레임 메모리 할당)
        return LString(strview(buffer, static_cast<usize>(ptr - buffer)));
    }

    template<typename Alloc>
    inline bool MGF3D::StringUtils::IsNullOrEmpty(const TString<Alloc>& s)
    {
        return s.empty();
    }

    inline constexpr char8 StringUtils::ToLower(char8 c)
    {
        if (CoreUtils::IsBetween(c, static_cast<char8>('A'), static_cast<char8>('Z')))
            return c + 32;
        return c;
    }

    inline constexpr char8 StringUtils::ToUpper(char8 c)
    {
        // 소문자 'a' ~ 'z' 범위에 있다면 대문자로 변환
        if (CoreUtils::IsBetween(c, static_cast<char8>('a'), static_cast<char8>('z')))
            return c - 32;
        return c;
    }

    inline int32 StringUtils::Compare(cstr s1, cstr s2)
    {
        if (s1 == s2) return 0;
        if (s1 == nullptr) return -1;
        if (s2 == nullptr) return 1;
        return Compare(strview(s1), strview(s2));
    }

    inline int32 StringUtils::Compare(strview s1, strview s2)
    {
        return s1.compare(s2);
    }

    inline bool StringUtils::Equals(strview s1, strview s2)
    {
        return s1 == s2;
    }
}