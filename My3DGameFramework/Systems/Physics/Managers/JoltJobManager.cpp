#include "PhysicsPch.h"
#include "JoltJobManager.h"

namespace MGF3D
{
	JoltJobManager::JoltJobManager() = default;
	JoltJobManager::~JoltJobManager() = default;

	bool JoltJobManager::Init()
	{
		// 1. 이중 초기화 방지
		if (m_jobSystem != nullptr)
		{
			MGF_LOG_WARN("[Physics] JoltJobManager is already initialized.");
			return true;
		}

		// 2. Jolt의 순정 Lock-free 스레드 풀 생성
		m_jobSystem = MakeUnique<::JPH::JobSystemThreadPool>
		(
			::JPH::cMaxPhysicsJobs,
			::JPH::cMaxPhysicsBarriers,
			-1
		);
		if (!m_jobSystem)
		{
			MGF_LOG_FATAL("[Physics] Failed to create Jolt JobSystemThreadPool!");
			return false;
		}

		// 사용 중인 실제 스레드 개수를 로그로 출력
		MGF_LOG_INFO
		(
			"[Physics] JoltJobManager Initialized with Jolt Internal Thread Pool. (Threads: {0})", 
			m_jobSystem->GetMaxConcurrency()
		);
		return true;
	}

	void JoltJobManager::Shutdown()
	{
		if (m_jobSystem) m_jobSystem.Reset();
		MGF_LOG_INFO("[Physics] JoltJobManager Shutdown.");
	}

	Ptr<JPH::JobSystem> JoltJobManager::GetJobSystem() const
	{
		return m_jobSystem.Get();
	}
}