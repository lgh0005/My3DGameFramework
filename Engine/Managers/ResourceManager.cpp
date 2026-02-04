#include "EnginePch.h"
#include "ResourceManager.h"
#include "Core/ResourceLoader.h"

bool ResourceManager::Init()
{
    // 1. 빌트인 리소스(Cube, Plane, Material 등) 생성
    if (!ResourceLoader::Init()) 
        return false;

    // 2. 기본 엔진 셰이더 로드
    std::string shaderConfigPath = FILE_MGR.Resolve("@EngineConfig/DefaultShaders.json");
    if (!ResourceLoader::Load(shaderConfigPath))
    {
        LOG_ERROR("ResourceManager: Failed to load Engine Shaders at '{}'", shaderConfigPath);
        return false;
    }

    // 3. 게임 리소스 로드
    std::string gameConfigPath = FILE_MGR.Resolve("@GameConfig/Resources.json");
    if (!ResourceLoader::Load(gameConfigPath))
    {
        LOG_WARN("ResourceManager: Game resource config not found at '{}'. Skipping.", gameConfigPath);
    }

    return true;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}