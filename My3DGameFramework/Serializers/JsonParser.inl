#pragma once

template<typename T>
inline T JsonParser::Get(const std::string& key, const T& defaultValue)
{
    return m_data.value(key, defaultValue);
}

template<typename T>
inline bool JsonParser::HasTypeOf(const std::string& key)
{
    if (!m_data.contains(key)) return false;
    const auto& val = m_data[key];

    // 1. 불리언
    if constexpr (std::is_same_v<T, bool>) return val.is_boolean();

    // 2. 문자열
    else if constexpr (std::is_same_v<T, std::string>) return val.is_string();

    // 3. nlohmann::json 구조 타입
    else if constexpr (std::is_same_v<T, nlohmann::json::object_t>)
        return val.is_object();
    else if constexpr (std::is_same_v<T, nlohmann::json::array_t>)
        return val.is_array();

    // 4. 기타 특수 타입
    else if constexpr (std::is_same_v<T, std::nullptr_t>)
        return val.is_null();
    else if constexpr (std::is_same_v<T, std::vector<uint8>>)
        return val.is_binary();

    // 5. 숫자
    else if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_unsigned_v<T>) return val.is_number_unsigned();
        else return val.is_number_integer();
    }
    else if constexpr (std::is_floating_point_v<T>) return val.is_number_float();

    return false;
}