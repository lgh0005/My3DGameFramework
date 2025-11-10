#pragma once

namespace ConverterUtils
{
    template<typename T>
    inline void WriteData(std::ofstream& file, const T& data)
    {
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    inline void WriteString(std::ofstream& file, const std::string& str)
    {
        uint32 len = (uint32)str.length();
        WriteData(file, len);
        file.write(str.c_str(), len);
    }
}