#pragma once

namespace MGF3D
{
    template<typename T>
    inline T JsonParser::Get(const String& key, const T& defaultValue)
    {
        auto ptr = nlohmann::json::json_pointer(key);
        if (m_data.contains(ptr)) return m_data.at(ptr).get<T>();
        return defaultValue;
    }

    template<typename T>
    inline bool JsonParser::HasTypeOf(const String& key)
    {
        auto ptr = nlohmann::json::json_pointer(key);
        if (!m_data.contains(ptr)) return false;

        const auto& val = m_data.at(ptr);

        if constexpr (std::is_same_v<T, bool>) return val.is_boolean();
        else if constexpr (std::is_same_v<T, String> || std::is_same_v<T, std::string>) return val.is_string();
        else if constexpr (std::is_same_v<T, nlohmann::json::object_t>) return val.is_object();
        else if constexpr (std::is_same_v<T, nlohmann::json::array_t>) return val.is_array();
        else if constexpr (std::is_same_v<T, std::nullptr_t>) return val.is_null();

        // 숫자 타입 체크 (정수/실수 분리)
        else if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_unsigned_v<T>) return val.is_number_unsigned();
            else return val.is_number_integer();
        }
        else if constexpr (std::is_floating_point_v<T>) return val.is_number_float();

        return false;
    }
}