#include "EnginePch.h"
#include "JsonParser.h"

DECLARE_DEFAULTS_IMPL(JsonParser)

bool JsonParser::LoadFromFile(const std::string& path)
{
	Clear();
	m_filePath = ResolvePath(path);

	if (!fs::exists(m_filePath))
	{
		m_lastErrorMessage = "JSON file not found: " + m_filePath;
		return false;
	}

	std::ifstream file(m_filePath);
	if (!file.is_open())
	{
		m_lastErrorMessage = "Failed to open JSON file: " + m_filePath;
		return false;
	}

	// JSON 로드 및 파싱
	m_jsonData = nlohmann::json::parse(file);

	m_isLoaded = true;
	LOG_INFO("JSON Loaded successfully: {}", m_filePath);
	return true;
}

void JsonParser::Clear()
{
	m_jsonData.clear();
	m_isLoaded = false;
	m_filePath.clear();
	m_lastErrorMessage.clear();
}

bool JsonParser::ValidateRoot(const std::string& rootKey)
{
	if (m_jsonData.find(rootKey) == m_jsonData.end())
	{
		m_lastErrorMessage = "Invalid JSON format: Missing root key '" + rootKey + "'";
		return false;
	}
	return true;
}