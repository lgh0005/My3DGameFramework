#include "EnginePch.h"
#include "ResourceManager.h"
#include "Core/ResourceLoader.h"
#include "Core/EngineConfigurator.h"

bool ResourceManager::Init()
{
    bool success = true;

    // 1. 빌트인 리소스(Cube, Plane, Material 등) 생성
    if (!ResourceLoader::Init()) 
        return false;

    // 2. 기본 엔진 셰이더 로드
    if (!ResourceLoader::Load(EngineConfigurator::GetShaderConfigPath().string()))
        return false;

    // 3. 게임 리소스 로드
    if (!ResourceLoader::Load(EngineConfigurator::GetGameConfigPath().string()))
        return false;

    return success;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}