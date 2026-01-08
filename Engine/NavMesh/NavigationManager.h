#pragma once
#include "NavMeshFunctors.h"

CLASS_PTR(NavigationManager)
class NavigationManager
{
public:
	~NavigationManager();
	static NavigationManagerUPtr Create();

	bool BuildNavMesh(const std::vector<float>& vertices, const std::vector<int>& indices);
	bool FindPath(const glm::vec3& startPos, const glm::vec3& endPos, std::vector<glm::vec3>& outPath);
	void Clean();

private:
	NavigationManager();
	bool Init();

	NavMeshUPtr       m_navMesh;
	NavMeshQueryUPtr  m_navQuery;
	RecastContextUPtr m_ctx;
};