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

RigidbodyUPtr Rigidbody::Create()
{
	return RigidbodyUPtr(new Rigidbody());
}

bool Rigidbody::CreateBody()
{
	// 이미 바디가 있다면 패스
	if (!m_bodyID.IsInvalid()) return false;

	// 1. 내 게임 오브젝트에 붙은 Collider 컴포넌트 찾기
	Collider* collider = GetOwner()->GetComponent<Collider>();
	if (collider == nullptr)
	{
		LOG_WARN
		(
			"Rigidbody creation deferred on GameObject '{}'. No Collider found yet.", 
			GetOwner()->GetName()
		);
		return false;
	}

	// 2. Collider에게 Jolt Shape 생성 요청
	ShapeRefC shape = collider->GetShape();
	if (shape == nullptr)
	{
		LOG_ERROR("Failed to create shape from Collider.");
		return false;
	}

	// 3. 초기 위치/회전 설정
	RVec3 initialPos = Utils::ToJoltVec3(GetTransform().GetWorldPosition());
	Quat initialRot = Utils::ToJoltQuat(GetTransform().GetWorldRotation());

	// 4. 바디 설정 구조체 채우기
	//    - Static이면 NON_MOVING 레이어, Dynamic이면 MOVING 레이어로 설정
	ObjectLayer layer = (m_motionType == EMotionType::Static) ? Layers::NON_MOVING : Layers::MOVING;

	BodyCreationSettings bodySettings(shape, initialPos, initialRot, m_motionType, layer);

	// 5. 물리 속성 적용
	bodySettings.mFriction = m_friction;
	bodySettings.mRestitution = m_restitution;
	bodySettings.mGravityFactor = m_useGravity ? 1.0f : 0.0f;

	// 질량 재정의 (Dynamic일 때만 유효)
	if (m_motionType == EMotionType::Dynamic)
	{
		bodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
		bodySettings.mMassPropertiesOverride.mMass = m_mass;
		if (m_rotationLocked)
			bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX | 
										JPH::EAllowedDOFs::TranslationY | 
										JPH::EAllowedDOFs::TranslationZ;
	}

	// 6. [중요] Trigger(Sensor) 여부 설정
	//    Collider가 "나 트리거야!"라고 하면 Jolt에게 "이건 센서다"라고 알려줌
	bodySettings.mIsSensor = collider->IsTrigger();

	// 7. [핵심] UserData에 GameObject 포인터 저장 (충돌 콜백용)
	bodySettings.mUserData = reinterpret_cast<uint64>(GetOwner());

	// 8. 실제 바디 생성 및 등록
	m_bodyID = GetBodyInterface().CreateAndAddBody(bodySettings, EActivation::Activate);

	return true;
}

void Rigidbody::SetMass(float mass)
{
	m_mass = mass;
	
	// 바디가 아직 생성되지 않았다면, 나중에 CreateBody 할 때 m_mass를 쓸 테니 리턴
	if (!IsValid()) return;

	// Jolt 시스템 가져오기 (PHYSICS 매크로 활용)
	const JPH::PhysicsSystem& physicsSystem = PHYSICS.GetPhysicsSystem();
	JPH::BodyInterface& bodyInterface = GetBodyInterface();

	// 1. 바디를 수정하기 위해 쓰기 권한(Write Lock)을 얻음
	JPH::BodyLockWrite lock(physicsSystem.GetBodyLockInterface(), m_bodyID);

	if (lock.Succeeded())
	{
		JPH::Body& body = lock.GetBody();

		// 2. Dynamic 바디가 아니면 질량이 의미가 없으므로 패스
		if (!body.IsRigidBody() || body.GetMotionType() != JPH::EMotionType::Dynamic)
			return;

		// 3. 현재 쉐이프(Shape)에서 기본 질량 특성(MassProperties)을 계산
		const JPH::Shape* shape = body.GetShape();
		JPH::MassProperties massProps = shape->GetMassProperties();

		// 4. 우리가 원하는 질량(m_mass)에 맞춰 관성(Inertia)을 스케일링
		// (예: 질량이 2배 되면, 회전 저항력도 2배가 되어야 자연스러움)
		massProps.ScaleToMass(m_mass);

		// 5. 재계산된 질량 특성을 바디에 적용
		// EAllowedDOFs::All -> 모든 축의 회전/이동 허용 상태 유지
		body.GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, massProps);
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
	m_motionType = motionType;
	if (IsValid()) GetBodyInterface().SetMotionType(m_bodyID, motionType, EActivation::Activate);
}

void Rigidbody::SetUseGravity(bool useGravity)
{
	m_useGravity = useGravity;
	if (IsValid()) GetBodyInterface().SetGravityFactor(m_bodyID, useGravity ? 1.0f : 0.0f);
}

void Rigidbody::SetRotationLock(bool locked)
{
	m_rotationLocked = locked;
	if (!IsValid()) return;

	JPH::BodyInterface& bodyInterface = GetBodyInterface();
	JPH::BodyLockWrite lock(PHYSICS.GetPhysicsSystem().GetBodyLockInterface(), m_bodyID);
	if (lock.Succeeded())
	{
		JPH::Body& body = lock.GetBody();
		 
		// 1. Static 바디나 Kinematic 바디는 MotionProperties가 없거나 설정할 필요 없음
		if (!body.IsRigidBody() || body.GetMotionType() != JPH::EMotionType::Dynamic)
			return;

		// 2. MotionProperties 가져오기 (없으면 리턴)
		JPH::MotionProperties* motionProps = body.GetMotionProperties();
		if (!motionProps) return;

		// 3. 현재 쉐이프에서 질량 특성 다시 계산 (SetMass 때와 동일 로직)
		// 관성(Inertia)을 DOF에 맞춰 다시 세팅해야 하기 때문에 필요함
		const JPH::Shape* shape = body.GetShape();
		JPH::MassProperties massProps = shape->GetMassProperties();
		massProps.ScaleToMass(m_mass); // 우리가 설정해둔 질량 적용

		// 4. 자유도(DOF) 결정 
		JPH::EAllowedDOFs allowedDOFs = JPH::EAllowedDOFs::All;
		if (m_rotationLocked)
		{
			// 이동(Translation) XYZ만 허용하고, 회전(Rotation)은 잠금
			allowedDOFs = JPH::EAllowedDOFs::TranslationX |
				JPH::EAllowedDOFs::TranslationY |
				JPH::EAllowedDOFs::TranslationZ;
		}

		// 5. ★ 핵심: 질량 특성과 자유도를 한 번에 설정
		// 이렇게 하면 Jolt가 "아, 이 축은 회전하면 안 되니까 관성을 무한대로 설정해야겠구나"라고 처리함
		motionProps->SetMassProperties(allowedDOFs, massProps);
	}
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

void Rigidbody::Start()
{
	CreateBody();
}

void Rigidbody::Update()
{
	// fallback of dependency of collider
	if (m_bodyID.IsInvalid()) CreateBody();
	if (IsValid()) Super::Update();
}
