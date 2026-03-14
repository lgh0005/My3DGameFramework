#include "CorePch.h"
#include "EntityManager.h"

namespace MGF3D
{
	EntityManager::EntityManager() = default;
	EntityManager::~EntityManager() = default;

	void EntityManager::OnPartition()
	{
		for (auto& pair : m_storages) 
			pair.second->Defragment();
	}
}