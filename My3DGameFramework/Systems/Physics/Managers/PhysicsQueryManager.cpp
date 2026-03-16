#include "PhysicsPch.h"
#include "PhysicsQueryManager.h"
#include "Managers/PhysicsManager.h"

namespace MGF3D
{
	PhysicsQueryManager::PhysicsQueryManager() = default;
	PhysicsQueryManager::~PhysicsQueryManager() = default;

	void PhysicsQueryManager::Init()
	{
		m_querySystem = &PhysicsManager::Instance().GetNarrowPhaseQuery();
		MGF_ASSERT(m_querySystem != nullptr, "Failed to hook NarrowPhaseQuery! PhysicsManager must be initialized BEFORE PhysicsQueryManager.");
		MGF_LOG_INFO("[Physics] PhysicsQueryManager Initialized (NarrowPhaseQuery Hooked).");
	}

	void PhysicsQueryManager::Shutdown()
	{
		m_querySystem = nullptr;
		MGF_LOG_INFO("[Physics] PhysicsQueryManager Shutdown.");
	}
}