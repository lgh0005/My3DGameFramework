#include "PhysicsPch.h"
#include "PhysicsManager.h"
#include "Managers/JoltMemoryManager.h"
#include "Managers/JoltJobManager.h"
#include "Managers/CollisionManager.h"
#include "Collision/JoltFilterSet.h"

namespace MGF3D
{
	PhysicsManager::PhysicsManager() = default;
	PhysicsManager::~PhysicsManager() = default;

	bool PhysicsManager::Init()
	{
		// 1. 물리 우주 본체 생성
		m_physicsSystem = UniquePtr<::JPH::PhysicsSystem>(new ::JPH::PhysicsSystem());
		MGF_ASSERT(m_physicsSystem != nullptr, "PhysicsManager: Failed to allocate JPH::PhysicsSystem memory!");

		// 2. Jolt 필터 세팅
		Ptr<JoltFilterSet> filter = MGF_COLLISION.GetFilter();
		MGF_ASSERT(filter != nullptr, "PhysicsManager: CollisionManager must be initialized BEFORE PhysicsManager!");

		m_physicsSystem->Init
		(
			JoltConfig::cMaxBodies,
			JoltConfig::cNumBodyMutexes,
			JoltConfig::cMaxBodyPairs,
			JoltConfig::cMaxContactConstraints,
			static_cast<JPH::BroadPhaseLayerInterface&>(*filter),	   // 1. BroadPhaseLayerInterface
			static_cast<JPH::ObjectVsBroadPhaseLayerFilter&>(*filter), // 2. ObjectVsBroadPhaseLayerFilter
			static_cast<JPH::ObjectLayerPairFilter&>(*filter)		   // 3. ObjectLayerPairFilter
		);

		// 4. 전역 물리 설정 적용
		m_physicsSystem->SetGravity(JoltMathBridge::ToJoltVec3(m_gravity));
		MGF_LOG_INFO("[Physics] PhysicsManager Initialized Successfully.");

		return true;
	}

	void PhysicsManager::Update(float deltaTime)
	{
		// 자동 시뮬레이션이 꺼져 있거나 시스템이 없으면 return
		if (!m_autoSimulation || m_physicsSystem == nullptr)
			return;

		const int32 cCollisionSteps = 1;
		m_physicsSystem->Update
		(
			deltaTime,
			cCollisionSteps,
			JOLT_MEMORY.GetTempAllocator(),
			JOLT_JOB.GetJobSystem()
		);
	}

	void PhysicsManager::Shutdown()
	{
		// 1. Jolt 자원 해제
		if (m_physicsSystem) m_physicsSystem.Reset();
		MGF_LOG_INFO("[Physics] PhysicsManager Shutdown.");
	}

	/*========================//
	//   Jolt API providers   //
	//========================*/
	JPH::BodyInterface& PhysicsManager::GetBodyInterface()
	{
		MGF_ASSERT(m_physicsSystem != nullptr, "PhysicsSystem is not initialized!");
		return m_physicsSystem->GetBodyInterface();
	}

	const JPH::NarrowPhaseQuery& PhysicsManager::GetNarrowPhaseQuery() const
	{
		MGF_ASSERT(m_physicsSystem != nullptr, "PhysicsSystem is not initialized!");
		return m_physicsSystem->GetNarrowPhaseQuery();
	}

	Ptr<JPH::PhysicsSystem> PhysicsManager::GetSystem() const
	{
		return m_physicsSystem.Get();
	}

	/*==========================//
	//   Global physics rules   //
	//==========================*/
	void PhysicsManager::SetGravity(const vec3& inGravity)
	{
		m_gravity = inGravity;
		if (m_physicsSystem) m_physicsSystem->SetGravity(JoltMathBridge::ToJoltVec3(inGravity));
	}

	vec3 PhysicsManager::GetGravity() const
	{
		if (m_physicsSystem) return JoltMathBridge::ToGLMVec3(m_physicsSystem->GetGravity());
		return m_gravity;
	}

	void PhysicsManager::SetAutoSimulation(bool inAuto)
	{
		m_autoSimulation = inAuto;
	}

	bool PhysicsManager::IsAutoSimulation() const
	{
		return m_autoSimulation;
	}
}

