#pragma once

namespace MGF3D
{
    template<typename T>
    inline T JsonParser::Get(const SString& key, const T& defaultValue) const
    {
        if (!m_data.contains(key.CStr())) return defaultValue;
        const auto& val = m_data[key.CStr()];
        if (!HasTypeOf<T>(key)) return defaultValue;
        return val.get<T>();
    }

    template<typename T>
    inline bool JsonParser::HasTypeOf(const SString& key) const
    {
        if (!m_data.contains(key.CStr())) return false;
        const auto& val = m_data[key.CStr()];

        // 1. 기본 타입 체크
        if constexpr (std::is_same_v<T, bool>) return val.is_boolean();
        else if constexpr (std::is_same_v<T, str>) return val.is_string();

        // 2. nlohmann::json 내부 구조 타입
        else if constexpr (std::is_same_v<T, nlohmann::json::object_t>) return val.is_object();
        else if constexpr (std::is_same_v<T, nlohmann::json::array_t>) return val.is_array();

        // 3. 숫자 타입 (정수/부동소수점 구분)
        else if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_unsigned_v<T>) return val.is_number_unsigned();
            else return val.is_number_integer();
        }
        else if constexpr (std::is_floating_point_v<T>) return val.is_number_float();

        return false;
    }
}
