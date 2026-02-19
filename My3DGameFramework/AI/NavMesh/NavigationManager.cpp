#include "EnginePch.h"
#include "NavigationManager.h"
#include <recastnavigation/Recast.h>
#include <recastnavigation/DetourNavMesh.h>
#include <recastnavigation/DetourNavMeshQuery.h>


DECLARE_DEFAULTS_IMPL(NavigationManager)

NavigationManagerUPtr NavigationManager::Create()
{
	auto navigator = NavigationManagerUPtr(new NavigationManager());
	if (!navigator->Init()) return nullptr;
	return std::move(navigator);
}

bool NavigationManager::Init()
{
	m_ctx = std::make_unique<rcContext>();
	if (!m_ctx) return false;
	return true;
}

bool NavigationManager::BuildNavMesh(const std::vector<float>& vertices, const std::vector<int>& indices)
{
	// TODO
	return true;
}

bool NavigationManager::FindPath(const glm::vec3& startPos, const glm::vec3& endPos, std::vector<glm::vec3>& outPath)
{
	// TODO
	return true;
}

void NavigationManager::Clean()
{
	m_navMesh.reset();
	m_navQuery.reset();
}