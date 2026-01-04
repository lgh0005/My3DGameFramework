#include "EnginePch.h"
#include "ResourceManager.h"
#include "Core/ResourceLoader.h"

bool ResourceManager::Init()
{
	// TODO : ResourceLoader에서 전역 리소스 로드
	return true;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}