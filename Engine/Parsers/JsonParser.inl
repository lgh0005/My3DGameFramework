#pragma once

template<typename T>
inline T JsonParser::Get(const std::string& key, const T& defaultValue)
{
    return m_data.value(key, defaultValue);
}
