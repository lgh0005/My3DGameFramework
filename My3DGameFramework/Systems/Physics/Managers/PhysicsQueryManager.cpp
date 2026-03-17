#include "PhysicsPch.h"
#include "PhysicsQueryManager.h"
#include "Managers/PhysicsManager.h"

namespace MGF3D
{
	PhysicsQueryManager::PhysicsQueryManager() = default;
	PhysicsQueryManager::~PhysicsQueryManager() = default;

	bool PhysicsQueryManager::Init()
	{
		m_querySystem = &MGF_PHYSICS.GetNarrowPhaseQuery();
		MGF_ASSERT(m_querySystem != nullptr, "Failed to hook NarrowPhaseQuery! PhysicsManager must be initialized BEFORE PhysicsQueryManager.");
		
		MGF_LOG_INFO("[Physics] PhysicsQueryManager Initialized (NarrowPhaseQuery Hooked).");
		return true;
	}

	void PhysicsQueryManager::Shutdown()
	{
		m_querySystem = nullptr;
		MGF_LOG_INFO("[Physics] PhysicsQueryManager Shutdown.");
	}
}