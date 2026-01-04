#include "EnginePch.h"
#include "ResourceLoader.h"
#include <nlohmann/json.hpp> 
using json = nlohmann::json;

void ResourceLoader::LoadFromManifest(const std::string& manifestPath)
{
	// 1. 파일 읽기
	std::ifstream file(manifestPath);
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open resource manifest: {}", manifestPath);
		return;
	}

	// 2. JSON 파싱
	json data;
	try
	{
		file >> data;
	}
	catch (json::parse_error& e)
	{
		LOG_ERROR("JSON Parse Error: {}", e.what());
		return;
	}

	// 3. 리소스 로드
}
