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

/*============================//
//   parsing helper methods   //
//============================*/
float YamlParser::LoadFloat(const YAML::Node& node, std::string_view tokenKey, float defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key]) return node[key].as<float>();
	return defaultValue;
}

bool YamlParser::LoadBool(const YAML::Node& node, std::string_view tokenKey, bool defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key]) return node[key].as<bool>();
	return defaultValue;
}

int YamlParser::LoadInt(const YAML::Node& node, std::string_view tokenKey, int defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key]) return node[key].as<int>();
	return defaultValue;
}

std::string YamlParser::LoadStr(const YAML::Node& node, std::string_view tokenKey, const std::string& defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key]) return node[key].as<std::string>();
	return defaultValue;
}

glm::vec2 YamlParser::LoadVec2(const YAML::Node& node, std::string_view tokenKey, const glm::vec2& defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key] && node[key].IsSequence() && node[key].size() >= 2)
	{
		return glm::vec2
		(
			node[key][0].as<float>(),
			node[key][1].as<float>()
		);
	}
	return defaultValue;
}

glm::vec3 YamlParser::LoadVec3(const YAML::Node& node, std::string_view tokenKey, const glm::vec3& defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key] && node[key].IsSequence() && node[key].size() >= 3)
	{
		return glm::vec3
		(
			node[key][0].as<float>(),
			node[key][1].as<float>(),
			node[key][2].as<float>()
		);
	}
	return defaultValue;
}

glm::vec4 YamlParser::LoadVec4(const YAML::Node& node, std::string_view tokenKey, const glm::vec4& defaultValue)
{
	std::string key = GetToken(Utils::StrHash(tokenKey.data()));
	if (!key.empty() && node[key] && node[key].IsSequence() && node[key].size() >= 3)
	{
		float r = node[key][0].as<float>();
		float g = node[key][1].as<float>();
		float b = node[key][2].as<float>();
		float a = (node[key].size() > 3) ? node[key][3].as<float>() : defaultValue.a;
		return glm::vec4(r, g, b, a);
	}
	return defaultValue;
}