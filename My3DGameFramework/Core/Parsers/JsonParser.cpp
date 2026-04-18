#include "CorePch.h"
#include "JsonParser.h"
#include "Managers/PathManager.h"

namespace MGF3D
{
	JsonParser::JsonParser() = default;
	JsonParser::~JsonParser() = default;

	bool JsonParser::LoadFromJsonFile(const String& path)
	{
		// 1. 가상 경로를 실제 물리 경로로 변환
		String resolvedPath = MGF_PATH.ResolvePath(path);

		// 2. json 파일 읽기
		InputFileStream file(path);
		if (!file.is_open()) return false;

		m_data = nlohmann::json::parse(file, nullptr, false);
		if (m_data.is_discarded())
		{
			MGF_LOG_ERROR("JsonParser: Failed to parse JSON in '{}'", resolvedPath);
			return false;
		}

		return true;
	}

	bool JsonParser::IsArray(const String& key) const
	{
		if (m_data.contains(nlohmann::json::json_pointer(key)))
			return m_data.at(nlohmann::json::json_pointer(key)).is_array();
		return false;
	}
}
