#include "EnginePch.h"
#include "ResourceManager.h"

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}