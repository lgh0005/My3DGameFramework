#include "EnginePch.h"
#include "EngineConfigurator.h"

bool EngineConfigurator::Init(const std::string& configPath)
{
	std::ifstream file(configPath);
	if (!file.is_open())
	{
		LOG_WARN("EngineConfigurator: Config file not found at '{}'. Using defaults.", configPath);
		return false; // 파일이 없어도 치명적 오류는 아님 (기본값 사용)
	}

	try
	{
		json root;
		file >> root;

		// 값이 존재할 때만 덮어쓰기 (기본값 보호)
		if (root.contains("resource_root")) s_resourceRoot = root["resource_root"].get<std::string>();
		if (root.contains("config_root")) s_configRoot = root["config_root"].get<std::string>();
		if (root.contains("shader_config")) s_shaderConfig = root["shader_config"].get<std::string>();
		if (root.contains("game_config"))   s_gameConfig = root["game_config"].get<std::string>();

		LOG_INFO("EngineConfigurator: Config loaded from '{}'", configPath);
		LOG_INFO("  - Resource Root: {}", s_resourceRoot.string());
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("EngineConfigurator: Failed to parse JSON. Error: {}", e.what());
		return false;
	}

	return true;
}