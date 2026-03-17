#include "PhysicsPch.h"
#include "PhysicsModule.h"
#include "Managers/JoltMemoryManager.h"
#include "Managers/JoltJobManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/PhysicsQueryManager.h"

namespace MGF3D
{
	PhysicsModule::PhysicsModule() = default;
	PhysicsModule::~PhysicsModule() = default;

	bool PhysicsModule::OnModuleInit()
	{
		MGF_LOG_INFO("[PhysicsModule] Booting up Physics Subsystems...");
		
		// 1. Jolt 기초 인프라 초기화
		MGF_INIT_SYS(JOLT_MEMORY.Init(), "JoltMemoryManager");
		MGF_INIT_SYS(JOLT_JOB.Init(), "JoltJobManager");
		
		// 2. Jolt API를 이용한 물리 매니저들 초기화
		MGF_INIT_SYS(MGF_COLLISION.Init(), "CollisionManager");
		MGF_INIT_SYS(MGF_PHYSICS.Init(), "PhysicsManager");
		MGF_INIT_SYS(MGF_PHYSICS_QUERY.Init(), "PhysicsQueryManager");

		return true;
	}

	void PhysicsModule::OnShutDown()
	{
		MGF_LOG_INFO("[PhysicsModule] Shutting down Physics Subsystems...");

		// 1. 물리 로직 매니저 해제
		MGF_SHUTDOWN_SYS(MGF_PHYSICS_QUERY.Shutdown(), "PhysicsQueryManager");
		MGF_SHUTDOWN_SYS(MGF_PHYSICS.Shutdown(), "PhysicsManager");
		MGF_SHUTDOWN_SYS(MGF_COLLISION.Shutdown(), "CollisionManager");

		// 2. Jolt 기초 인프라 해제
		MGF_SHUTDOWN_SYS(JOLT_JOB.Shutdown(), "JoltJobManager");
		MGF_SHUTDOWN_SYS(JOLT_MEMORY.Shutdown(), "JoltMemoryManager");

		MGF_LOG_INFO("[PhysicsModule] Physics Subsystems shutdown completed.");
	}
}