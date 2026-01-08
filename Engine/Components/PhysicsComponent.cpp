#include "EnginePch.h"
#include "PhysicsComponent.h"
#include "Components/Transform.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h> 
#include <Jolt/Physics/Body/BodyInterface.h>
using namespace JPH;

PhysicsComponent::PhysicsComponent() = default;
PhysicsComponent::~PhysicsComponent()
{
	DestroyBody();
}

/*========================//
//   life-cycle methods   //
//========================*/
void PhysicsComponent::OnEnable()
{
	// SetEnable(true)가 호출되거나, 객체가 활성화될 때 실행
	CreateBody();
}

void PhysicsComponent::OnDisable()
{
	// SetEnable(false)가 호출되거나, 객체가 비활성화될 때 실행
	DestroyBody();
}

void PhysicsComponent::OnDestroy()
{
	// 컴포넌트가 완전히 삭제될 때 실행
	DestroyBody();
}

void PhysicsComponent::Update()
{
	// 매 프레임 실행 (렌더링 직전 위치 동기화)
	// 만약 물리 연산이 튀는 게 싫다면 FixedUpdate()가 아니라 Update()에서 
	// 보간(Interpolation)된 위치를 가져오는 것이 부드러움.
	SyncPhysicsToTransform();
}

/*============================//
//   internal logic methods   //
//============================*/
void PhysicsComponent::DestroyBody()
{
	if (m_bodyID.IsInvalid()) return;

	BodyInterface& bodyInterface = GetBodyInterface();
	bodyInterface.RemoveBody(m_bodyID);
	bodyInterface.DestroyBody(m_bodyID);

	// ID 초기화
	m_bodyID = BodyID();
}

void PhysicsComponent::SetPosition(const glm::vec3& position)
{
	GetTransform().SetPosition(position);
	SyncTransformToPhysics();
}

void PhysicsComponent::SetRotation(const glm::quat& rotation)
{
	GetTransform().SetRotation(rotation);
	SyncTransformToPhysics();
}

void PhysicsComponent::SyncPhysicsToTransform()
{
	// [P -> T] 시뮬레이션 결과 반영 (Update용)
	if (m_bodyID.IsInvalid()) return;
	JPH::BodyInterface& bodyInterface = GetBodyInterface();

	// [최적화 & 버그 방지]
	// Dynamic(물리 힘을 받는 애)이 아니면 굳이 물리 엔진 위치를 가져올 필요가 없음.
	// Static: 움직이지 않음.
	// Kinematic: Transform이 주도권을 가짐 (T -> P 방향이어야 함).
	if (!bodyInterface.IsActive(m_bodyID) || bodyInterface.GetMotionType(m_bodyID) != JPH::EMotionType::Dynamic)
		return;

	// 1. 물리 엔진 좌표 가져오기
	JPH::RVec3 position = bodyInterface.GetPosition(m_bodyID);
	JPH::Quat rotation = bodyInterface.GetRotation(m_bodyID);

	// 2. 내 Transform에 적용
	Transform& transform = GetTransform();
	transform.SetWorldPosition(Utils::ToGlmVec3(position));
	transform.SetWorldRotation(Utils::ToGlmQuat(rotation));
}

void PhysicsComponent::SyncTransformToPhysics()
{
	// [T -> P] 초기화 또는 강제 이동 반영
	if (m_bodyID.IsInvalid()) return;

	Transform& transform = GetTransform();
	JPH::BodyInterface& bodyInterface = GetBodyInterface();

	//// 1. 현재 Transform 좌표를 Jolt로 변환
	JPH::RVec3 jPos = Utils::ToJoltVec3(transform.GetWorldPosition());
	JPH::Quat jRot = Utils::ToJoltQuat(transform.GetWorldRotation());

	// 2. 물리 엔진 위치 강제 설정 (텔레포트)
	// Activate flag: 이동 후 잠자던 물체를 깨울 것인가? (보통 Yes)
	bodyInterface.SetPositionAndRotation
	(
		m_bodyID, 
		jPos, jRot, 
		JPH::EActivation::Activate
	);
}

JPH::BodyInterface& PhysicsComponent::GetBodyInterface() const
{
	return PHYSICS.GetBodyInterface();
}