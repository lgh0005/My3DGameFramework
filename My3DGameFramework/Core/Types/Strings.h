#pragma once
#include <string>
#include <string_view>

namespace MGF3D
{
    // 기본 문자열
    using cstr = const char*;
    using String = std::string;
    using WString = std::wstring;

    // 기본 문자열 뷰
    using StringView = std::string_view;
    using WStringView = std::wstring_view;
}