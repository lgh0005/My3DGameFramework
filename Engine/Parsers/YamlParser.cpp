#include "EnginePch.h"
#include "YamlParser.h"

DECLARE_DEFAULTS_IMPL(YamlParser)

bool YamlParser::LoadFromFile(const std::string& path)
{
	Clear();
	m_filePath = ResolvePath(path);

	// 1. 파일 존재 여부 확인
	if (!fs::exists(m_filePath))
	{
		m_lastErrorMessage = "File not found: " + m_filePath;
		return false;
	}

	// 2. YAML 로드
	m_rootNode = YAML::LoadFile(m_filePath);
	if (!m_rootNode.IsDefined() || m_rootNode.IsNull())
	{
		m_lastErrorMessage = "Failed to parse or empty YAML file: " + m_filePath;
		return false;
	}

	m_isLoaded = true;
	LOG_INFO("YAML Loaded successfully: {}", m_filePath);
	return true;
}

void YamlParser::Clear()
{
	m_rootNode = YAML::Node();
	m_isLoaded = false;
	m_filePath.clear();
	m_lastErrorMessage.clear();
}

bool YamlParser::ValidateRoot(const std::string& rootKey)
{
	if (!m_rootNode[rootKey])
	{
		m_lastErrorMessage = "Invalid format: Missing root key '" + rootKey + "'";
		return false;
	}
	return true;
}
