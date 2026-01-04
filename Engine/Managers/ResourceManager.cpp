#include "EnginePch.h"
#include "ResourceManager.h"
#include "Core/ResourceLoader.h"

bool ResourceManager::Init()
{
    bool success = true;

    // TODO : 경로 하드코딩 처리 필요

    // 1. 빌트인 리소스(Cube, Plane, Material 등) 생성
    if (!ResourceLoader::Init()) 
        return false;

    // 2. 기본 엔진 셰이더 로드
    if (!ResourceLoader::Load("./Resources/Shaders/DefaultShaders.json"))
        return false;

    // 3. 게임 리소스 로드
    if (!ResourceLoader::Load("./Resources/Resources.json"))
        return false;

    return success;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}