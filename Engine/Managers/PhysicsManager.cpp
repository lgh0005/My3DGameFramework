#include "EnginePch.h"
#include "PhysicsManager.h"
#include "Misc/PhysicsHelper.h"
#include <Jolt/RegisterTypes.h>
using namespace JPH;

void PhysicsManager::Init()
{
	// 1. Jolt 기본 할당기 및 팩토리 등록
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();

	// 2. 정적 콜백 연결 : PhysicsHelper에 있는 PhysicsCallbacks을 이용
	Trace = PhysicsCallbacks::TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
	AssertFailed = PhysicsCallbacks::AssertFailedImpl;
#endif

	// 3. 메모리 할당기 생성
	m_TempAllocator = new TempAllocatorImpl(MAX_PHYSICS_MEMORY_ALLOC_VALUE);

	// 4. JobSystem 생성 (멀티스레딩)
	m_JobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	// 5. 설정 객체(레이어, 필터) 생성
	m_BPLayerInterface = new BPLayerInterfaceImpl();
	m_ObjectVsBroadPhaseLayerFilter = new ObjectVsBroadPhaseLayerFilterImpl();
	m_ObjectLayerPairFilter = new ObjectLayerPairFilterImpl();

	// 6. 리스너 생성
	m_BodyActivationListener = new MyBodyActivationListener();
	m_ContactListener = new MyContactListener();

	// 7. PhysicsSystem 생성 및 초기화
	m_PhysicsSystem = new PhysicsSystem();
	m_PhysicsSystem->Init
	(
		JoltConfig::cMaxBodies,
		JoltConfig::cNumBodyMutexes,
		JoltConfig::cMaxBodyPairs,
		JoltConfig::cMaxContactConstraints,
		*m_BPLayerInterface,
		*m_ObjectVsBroadPhaseLayerFilter,
		*m_ObjectLayerPairFilter
	);

	// 8. 리스너 등록
	m_PhysicsSystem->SetBodyActivationListener(m_BodyActivationListener);
	m_PhysicsSystem->SetContactListener(m_ContactListener);

	SPDLOG_INFO("Jolt Physics Initialized Successfully.");
}

void PhysicsManager::Update()
{
	// 물리 시뮬레이션 업데이트
	m_PhysicsSystem->Update
	(
		TIME.GetFixedDeltaTime(),
		1, // 충돌 단계 수 (1이면 보통 충분)
		m_TempAllocator, 
		m_JobSystem
	);
}

void PhysicsManager::Clear()
{
	// 1. Physics System 해제
	if (m_PhysicsSystem)
	{
		delete m_PhysicsSystem;
		m_PhysicsSystem = nullptr;
	}

	// 2. 리스너 및 필터 해제
	if (m_BodyActivationListener) { delete m_BodyActivationListener; m_BodyActivationListener = nullptr; }
	if (m_ContactListener) { delete m_ContactListener; m_ContactListener = nullptr; }

	if (m_ObjectLayerPairFilter) { delete m_ObjectLayerPairFilter; m_ObjectLayerPairFilter = nullptr; }
	if (m_ObjectVsBroadPhaseLayerFilter) { delete m_ObjectVsBroadPhaseLayerFilter; m_ObjectVsBroadPhaseLayerFilter = nullptr; }
	if (m_BPLayerInterface) { delete m_BPLayerInterface; m_BPLayerInterface = nullptr; }

	// 3. Core Objects 해제
	if (m_JobSystem) { delete m_JobSystem; m_JobSystem = nullptr; }
	if (m_TempAllocator) { delete m_TempAllocator; m_TempAllocator = nullptr; }

	// 4. Jolt 전역 해제
	UnregisterTypes();
	if (Factory::sInstance)
	{
		delete Factory::sInstance;
		Factory::sInstance = nullptr;
	}

	SPDLOG_INFO("Jolt Physics Cleared.");
}

BodyInterface& PhysicsManager::GetBodyInterface() const
{
	// PhysicsSystem이 초기화되지 않았을 때 호출하면 크래시가 나므로 주의
	JPH_ASSERT(m_PhysicsSystem != nullptr);
	return m_PhysicsSystem->GetBodyInterface();
}
