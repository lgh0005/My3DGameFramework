#include "EnginePch.h"
#include "LuaParser.h"
#include <nlohmann/json.hpp>

DECLARE_DEFAULTS_IMPL(LuaParser)

bool LuaParser::LoadConfig(const std::string& configPath)
{
	// 1. JSON 파일 로드 (부모의 기능 활용)
	if (!JsonParser::LoadFromFile(configPath)) return false;

	// 2. 토큰 맵 채우기 (Parser 베이스 클래스의 철학 유지)
	// 예: { "Tokens": { "TRANSFORM_API": "Transform", "INPUT_API": "Input" } }
	// TODO : 이 규칙들은 조금씩 정해야 함.
	if (m_jsonData.contains("Tokens"))
	{
		for (auto& [key, value] : m_jsonData["Tokens"].items())
		{
			uint32 hash = Utils::StrHash(key);
			m_tokenMap[hash] = value.get<std::string>();
		}
	}

	return true;
}

bool LuaParser::LoadFromFile(const std::string& path)
{
	// 실제 루아 실행 환경 설정을 로드 (예: 어떤 모듈을 켜고 끌 것인가)
	if (!JsonParser::LoadFromFile(path)) return false;

	ExtractLuaSettings();
	return true;
}

void LuaParser::ExtractLuaSettings()
{
	m_activeModules.clear();

	// JSON 예시: { "Modules": { "Transform": true, "Physics": false } }
	// TODO : 이 규칙들은 조금씩 정해야 함.
	if (m_jsonData.contains("Modules"))
	{
		for (auto& [name, active] : m_jsonData["Modules"].items())
		{
			if (active.get<bool>())
			{
				m_activeModules.push_back(name);
			}
		}
	}
}

void LuaParser::Clear()
{
	JsonParser::Clear();
	m_activeModules.clear();
	m_tokenMap.clear();
}