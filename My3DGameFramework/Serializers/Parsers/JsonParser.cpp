#include "EnginePch.h"
#include "JsonParser.h"

DECLARE_DEFAULTS_IMPL(JsonParser)

bool JsonParser::LoadFromJsonFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) return false;

	m_data = nlohmann::json::parse(file);
	return true;
}

bool JsonParser::IsArray(const std::string& key) const
{
	return m_data.contains(key) && m_data[key].is_array();
}
