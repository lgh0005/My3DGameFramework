#include "CorePch.h"
#include <fstream>
#include "JsonParser.h"

namespace MGF3D
{
	JsonParser::JsonParser() = default;
	JsonParser::~JsonParser() = default;

	bool JsonParser::LoadFromJsonFile(const MGFPath& path)
	{
        // 1. 파일 스트림 열기
        std::ifstream file(path.GetCStr());
        if (!file.is_open())
        {
            MGF_LOG_ERROR("JsonParser: Failed to open file: {}", path.GetCStr());
            return false;
        }

        // 2. 예외 없이 파싱 수행 (callback: nullptr, allow_exceptions: false)
        m_data = nlohmann::json::parse(file, nullptr, false);

        // 3. 파싱 결과 유효성 체크
        if (m_data.is_discarded())
        {
            MGF_LOG_ERROR("JsonParser: Syntax error in JSON: {}", path.GetCStr());
            return false;
        }

        return true;
	}

	bool JsonParser::IsArray(const SString& key) const
	{
		return Contains(key) && m_data[key.CStr()].is_array();
	}

	bool JsonParser::Contains(const SString& key) const
	{
		return m_data.contains(key.CStr());
	}
}