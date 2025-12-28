#include "EnginePch.h"
#include "PhysicsComponent.h"
#include "Components/Transform.h"

PhysicsComponent::~PhysicsComponent() = default;

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

	// 1. 물리 엔진 좌표 가져오기
	JPH::RVec3 position = bodyInterface.GetPosition(m_bodyID);
	JPH::Quat rotation = bodyInterface.GetRotation(m_bodyID);

	// 2. 내 Transform에 적용
	Transform& transform = GetTransform();
	transform.SetPosition(Utils::ToGlmVec3(position));
	transform.SetRotation(Utils::ToGlmQuat(rotation));
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