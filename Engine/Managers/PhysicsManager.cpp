#include "EnginePch.h"
#include "PhysicsManager.h"

#include "Graphics/Ray.h"
#include "Graphics/RaycastHit.h"

#include "Physics/PhysicsConfig.h"
#include "Physics/PhysicsCallbacks.h"
#include "Physics/BPLayerInterface.h"
#include "Physics/PhysicsFilters.h"
#include "Physics/PhysicsBodyActivationListener.h"
#include "Physics/PhysicsContactListener.h"
#include "Physics/JoltDebugRenderer.h"
#include "Physics/JoltGizmo.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
using namespace JPH;

DECLARE_DEFAULTS_IMPL(PhysicsManager)

void PhysicsManager::Init()
{
	// 1. Jolt 기본 할당기 및 팩토리 등록 및 정적 콜백 연결
	RegisterDefaultAllocator();
	Trace = PhysicsCallbacks::TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
	AssertFailed = PhysicsCallbacks::AssertFailedImpl;
#endif
	Factory::sInstance = new Factory();
	RegisterTypes();

	// 3. 메모리 할당기 생성 (10MB)
	m_TempAllocator = std::make_unique<TempAllocatorImpl>(MAX_PHYSICS_MEMORY_ALLOC_VALUE);

	// 4. JobSystem 생성 (스레드 풀)
	m_JobSystem = std::make_unique<JobSystemThreadPool>
	(
		JoltConfig::cMaxPhysicsJobs,
		JoltConfig::cMaxPhysicsBarriers,
		std::thread::hardware_concurrency() - 1
	);

	// 5. 설정 객체(레이어, 필터) 생성
	m_BPLayerInterface = std::make_unique<BPLayerInterfaceImpl>();
	m_ObjectVsBroadPhaseLayerFilter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
	m_ObjectLayerPairFilter = std::make_unique<ObjectLayerPairFilterImpl>();

	// 6. 리스너 생성
	m_BodyActivationListener = std::make_unique<PhysicsBodyActivationListener>();
	m_ContactListener = std::make_unique<PhysicsContactListener>();

	// 7. PhysicsSystem 생성 및 초기화
	m_PhysicsSystem = std::make_unique<PhysicsSystem>();
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
	m_PhysicsSystem->SetBodyActivationListener(m_BodyActivationListener.get());
	m_PhysicsSystem->SetContactListener(m_ContactListener.get());

	// 9. 디버깅 멤버 초기화
	m_gizmo = JoltGizmo::Create();
	m_debugRenderer = JoltDebugRenderer::Create(m_gizmo.get());

	LOG_INFO("Jolt Physics Initialized Successfully.");
}

void PhysicsManager::Update()
{
	// 1. 물리 시뮬레이션 업데이트
	m_PhysicsSystem->Update
	(
		TIME.GetFixedDeltaTime(),
		1, // 충돌 단계 수 (1이면 보통 충분)
		m_TempAllocator.get(),
		m_JobSystem.get()
	);

	// 2. 충돌 이벤트 처리
	if (m_ContactListener) m_ContactListener->DispatchEvents();
}

void PhysicsManager::Clear()
{
	// 1. 의존성 역순으로 안전하게 해제 (reset 호출)
	m_PhysicsSystem.reset();

	// 리스너 및 헬퍼들 정리
	m_BodyActivationListener.reset();
	m_ContactListener.reset();
	m_ObjectLayerPairFilter.reset();
	m_ObjectVsBroadPhaseLayerFilter.reset();
	m_BPLayerInterface.reset();

	// 디버그 객체들 정리
	m_debugRenderer.reset();
	m_gizmo.reset();

	// Core 시스템 정리
	m_JobSystem.reset();
	m_TempAllocator.reset();

	// Jolt 전역 해제
	UnregisterTypes();
	if (Factory::sInstance)
	{
		delete Factory::sInstance;
		Factory::sInstance = nullptr;
	}

	LOG_INFO("PhysicsManager : Jolt Physics Cleared.");
}

BodyInterface& PhysicsManager::GetBodyInterface() const
{
	// PhysicsSystem이 초기화되지 않았을 때 호출하면 크래시가 나므로 주의
	JPH_ASSERT(m_PhysicsSystem != nullptr);
	return m_PhysicsSystem->GetBodyInterface();
}

/*========================//
//   raycasting methods   //
//========================*/
bool PhysicsManager::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& outHit)
{
	// 1. 초기화 (실패 시 데이터 초기화)
	outHit.Reset();

	if (!m_PhysicsSystem) return false;

	// 2. Jolt 데이터 변환
	JPH::RVec3 start = Utils::ToJoltVec3(origin);
	JPH::Vec3 dir = Utils::ToJoltVec3(direction * maxDistance);
	JPH::RRayCast ray(start, dir);

	// 3. 레이 발사
	JPH::RayCastResult hit;
	const JPH::NarrowPhaseQuery& query = m_PhysicsSystem->GetNarrowPhaseQuery();

	if (query.CastRay(ray, hit))
	{
		// 4. 결과 추출
		// 4-1. 거리 및 충돌 지점
		float hitDistance = hit.mFraction * maxDistance;
		JPH::RVec3 hitPoint = ray.GetPointOnRay(hit.mFraction);

		// 4-2. 법선 벡터 (Jolt BodyInterface 필요)
		JPH::BodyInterface& bodyInterface = GetBodyInterface();
		JPH::Vec3 normal = bodyInterface.GetShape(hit.mBodyID)->GetSurfaceNormal(hit.mSubShapeID2, hitPoint);

		// 4-3. GameObject 찾기 (UserData)
		uint64 userData = bodyInterface.GetUserData(hit.mBodyID);
		GameObject* hitObj = reinterpret_cast<GameObject*>(userData);

		// 5. 님의 RaycastHit 클래스에 데이터 채우기
		outHit.SetHitResult
		(
			hitObj,
			Utils::ToGlmVec3(hitPoint),
			Utils::ToGlmVec3(normal),
			hitDistance
		);

		return true;
	}

	return false;
}


/*=======================//
//   debugging methods   //
//=======================*/
void PhysicsManager::DrawDebugData()
{
#ifdef defined(DEBUG) || defined(_DEBUG)
	if (!m_PhysicsSystem || !m_debugRenderer) return;

	// 그리기 옵션 설정
	JPH::BodyManager::DrawSettings settings;
	settings.mDrawShape = true;          // 충돌체 외형 (Mesh/Box/Sphere 등)
	settings.mDrawBoundingBox = false;   // AABB 박스 (필요하면 true)
	settings.mDrawVelocity = false;      // 속도 벡터
	settings.mDrawCenterOfMassTransform = false; // 무게 중심 좌표계

	// ★ Jolt 시스템이 내부의 모든 바디를 순회하며 
	// m_debugRenderer->DrawLine() 등을 호출하게 만듦
	m_PhysicsSystem->DrawBodies(settings, m_debugRenderer.get());
#endif
}

JoltGizmo* PhysicsManager::GetGizmo()
{
	return m_gizmo.get();
}

