#include "EnginePch.h"
#include "Rigidbody.h"
#include "Scene/GameObject.h"
#include "Components/Collider.h"
#include "Components/Transform.h"
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/MotionProperties.h>
using namespace JPH;

DECLARE_DEFAULTS_IMPL(Rigidbody)

/*==============================//
//   default rigidbody methods  //
//==============================*/
RigidbodyUPtr Rigidbody::Create()
{
	return RigidbodyUPtr(new Rigidbody());
}

void Rigidbody::Start()
{
	CreateBody();
}

void Rigidbody::Update()
{
	if (m_isMotionPropsDirty)
	{
		ApplyMotionProperties();
		m_isMotionPropsDirty = false;
	}

	Super::Update();
}

bool Rigidbody::CreateBody()
{
	// 이미 바디가 있다면 패스
	if (!m_bodyID.IsInvalid()) return false;

	// 1. 내 게임 오브젝트에 붙은 Collider 컴포넌트 찾기
	Collider* collider = GetOwner()->GetComponent<Collider>();
	if (collider == nullptr) return false;

	// 2. Collider에게 Jolt Shape 생성 요청
	ShapeRefC shape = collider->GetShape();
	if (shape == nullptr) return false;

	// 3. 초기 위치/회전 설정
	RVec3 initialPos = Utils::ToJoltVec3(GetTransform().GetWorldPosition());
	Quat initialRot = Utils::ToJoltQuat(GetTransform().GetWorldRotation());

	// 4. 바디 설정 구조체 채우기
	// Static이면 NON_MOVING 레이어, Dynamic이면 MOVING 레이어로 설정
	ObjectLayer layer = (m_motionType == EMotionType::Static) ? Layers::NON_MOVING : Layers::MOVING;

	BodyCreationSettings bodySettings(shape, initialPos, initialRot, m_motionType, layer);

	// 5. 물리 속성 적용
	bodySettings.mFriction = m_friction;
	bodySettings.mRestitution = m_restitution;
	bodySettings.mGravityFactor = m_useGravity ? 1.0f : 0.0f;
	bodySettings.mIsSensor = collider->IsTrigger();
	bodySettings.mUserData = reinterpret_cast<uint64>(GetOwner());

	// 4. 질량 설정 (Dynamic일 때만 유효)
	if (m_motionType == EMotionType::Dynamic)
	{
		bodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
		bodySettings.mMassPropertiesOverride.mMass = m_mass;
	}

	// 8. 실제 바디 생성 및 등록
	m_bodyID = GetBodyInterface().CreateAndAddBody(bodySettings, EActivation::Activate);

	// 6. [중요] 생성 직후 제약 조건(Constraints) 적용
	if (m_motionType == EMotionType::Dynamic) ApplyMotionProperties();
	
	m_isMotionPropsDirty = false;

	return true;
}

void Rigidbody::ApplyMotionProperties()
{
	// 1. 시스템 가져오기
	const JPH::PhysicsSystem& physicsSystem = PHYSICS.GetPhysicsSystem();
	JPH::BodyInterface& bodyInterface = GetBodyInterface();

	// 2. 쓰기 락
	JPH::BodyLockWrite lock(physicsSystem.GetBodyLockInterface(), m_bodyID);
	if (!lock.Succeeded()) return;

	JPH::Body& body = lock.GetBody();

	// 3. Dynamic이 아니면 질량/관성 설정 불가 -> 리턴
	if (!body.IsRigidBody() || body.GetMotionType() != JPH::EMotionType::Dynamic)
		return;

	// 4. 질량 특성 계산
	const JPH::Shape* shape = body.GetShape();
	JPH::MassProperties massProps = shape->GetMassProperties();
	massProps.ScaleToMass(m_mass);

	// 5. 자유도(DOF) 결정 (회전 잠금 여부 반영)
	JPH::EAllowedDOFs allowedDOFs = JPH::EAllowedDOFs::All;
	if (m_constraints.IsSet(RigidbodyConstraint::FreezePositionX))
		allowedDOFs &= ~EAllowedDOFs::TranslationX;
	if (m_constraints.IsSet(RigidbodyConstraint::FreezePositionY))
		allowedDOFs &= ~EAllowedDOFs::TranslationY;
	if (m_constraints.IsSet(RigidbodyConstraint::FreezePositionZ))
		allowedDOFs &= ~EAllowedDOFs::TranslationZ;

	if (m_constraints.IsSet(RigidbodyConstraint::FreezeRotationX))
		allowedDOFs &= ~EAllowedDOFs::RotationX;
	if (m_constraints.IsSet(RigidbodyConstraint::FreezeRotationY))
		allowedDOFs &= ~EAllowedDOFs::RotationY;
	if (m_constraints.IsSet(RigidbodyConstraint::FreezeRotationZ))
		allowedDOFs &= ~EAllowedDOFs::RotationZ;

	// 6. 최종 적용 (질량 + 자유도)
	body.GetMotionProperties()->SetMassProperties(allowedDOFs, massProps);
}

/*==================================//
//   default rigidbody properties   //
//==================================*/
void Rigidbody::SetMass(float mass)
{
	if (m_mass != mass) // 값이 다를 때만 더티 마킹
	{
		m_mass = mass;
		m_isMotionPropsDirty = true;
	}
}

void Rigidbody::SetFriction(float friction)
{
	m_friction = friction;
	if (IsValid()) GetBodyInterface().SetFriction(m_bodyID, friction);
}

void Rigidbody::SetRestitution(float restitution)
{
	m_restitution = restitution;
	if (IsValid()) GetBodyInterface().SetRestitution(m_bodyID, restitution);
}

void Rigidbody::SetMotionType(JPH::EMotionType motionType)
{
	if (m_motionType != motionType)
	{
		m_motionType = motionType;
		if (IsValid())
		{
			GetBodyInterface().SetMotionType(m_bodyID, motionType, EActivation::Activate);
			if (motionType == EMotionType::Dynamic) m_isMotionPropsDirty = true;
		}
	}
}

void Rigidbody::SetUseGravity(bool useGravity)
{
	m_useGravity = useGravity;
	if (IsValid()) GetBodyInterface().SetGravityFactor(m_bodyID, useGravity ? 1.0f : 0.0f);
}

/*===========================//
//   rigidbody constraints   //
//===========================*/
void Rigidbody::SetConstraints(const RigidbodyConstraint& constraints)
{
	if (m_constraints != constraints)
	{
		m_constraints = constraints;
		m_isMotionPropsDirty = true;
	}
}

void Rigidbody::FreezePosition(bool x, bool y, bool z)
{
	RigidbodyConstraint old = m_constraints;

	if (x) m_constraints.Set(RigidbodyConstraint::FreezePositionX);
	else   m_constraints.Unset(RigidbodyConstraint::FreezePositionX);

	if (y) m_constraints.Set(RigidbodyConstraint::FreezePositionY);
	else   m_constraints.Unset(RigidbodyConstraint::FreezePositionY);

	if (z) m_constraints.Set(RigidbodyConstraint::FreezePositionZ);
	else   m_constraints.Unset(RigidbodyConstraint::FreezePositionZ);

	// 값이 바뀌었을 때만 마킹
	if (m_constraints != old) m_isMotionPropsDirty = true;
}

void Rigidbody::FreezeRotation(bool x, bool y, bool z)
{
	RigidbodyConstraint old = m_constraints;

	if (x) m_constraints.Set(RigidbodyConstraint::FreezeRotationX);
	else   m_constraints.Unset(RigidbodyConstraint::FreezeRotationX);

	if (y) m_constraints.Set(RigidbodyConstraint::FreezeRotationY);
	else   m_constraints.Unset(RigidbodyConstraint::FreezeRotationY);

	if (z) m_constraints.Set(RigidbodyConstraint::FreezeRotationZ);
	else   m_constraints.Unset(RigidbodyConstraint::FreezeRotationZ);

	if (m_constraints != old) m_isMotionPropsDirty = true;
}

/*===========================//
//   force & velocity        //
//===========================*/
void Rigidbody::AddForce(const glm::vec3& force)
{
	if (!IsValid()) return;
	// [중요] 힘은 "지속적인 가속"을 줄 때 사용 (예: 로켓 추진, 바람, 중력장)
	// F = ma에 따라 가속도가 붙음.
	// Jolt는 AddForce 호출 시 자동으로 바디를 깨웁니다 (Activate).
	GetBodyInterface().AddForce(m_bodyID, Utils::ToJoltVec3(force));
}

void Rigidbody::AddImpulse(const glm::vec3& impulse)
{
	// 바디가 없으면 무시
	if (!IsValid()) return;

	// [중요] 임펄스는 "순간적인 충격"을 줄 때 사용 (예: 점프, 총알 피격, 폭발)
	// 속도가 순간적으로 변함 (Velocity += Impulse / Mass)
	GetBodyInterface().AddImpulse(m_bodyID, Utils::ToJoltVec3(impulse));
}

glm::vec3 Rigidbody::GetLinearVelocity() const
{
	if (!IsValid()) return glm::vec3(0.0f);
	return Utils::ToGlmVec3(GetBodyInterface().GetLinearVelocity(m_bodyID));
}

void Rigidbody::SetLinearVelocity(const glm::vec3& velocity)
{
	if (!IsValid()) return;
	GetBodyInterface().SetLinearVelocity(m_bodyID, Utils::ToJoltVec3(velocity));
}

glm::vec3 Rigidbody::GetAngularVelocity() const
{
	if (!IsValid()) return glm::vec3(0.0f);
	return Utils::ToGlmVec3(GetBodyInterface().GetAngularVelocity(m_bodyID));
}

void Rigidbody::SetAngularVelocity(const glm::vec3& velocity)
{
	if (!IsValid()) return;
	GetBodyInterface().SetAngularVelocity(m_bodyID, Utils::ToJoltVec3(velocity));
}