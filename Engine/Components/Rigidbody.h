#pragma once
#include "PhysicsComponent.h"
#include <Jolt/Physics/Body/MotionType.h>

CLASS_PTR(Rigidbody)
class Rigidbody : public PhysicsComponent
{
	using Super = PhysicsComponent;

public:
	virtual ~Rigidbody();
	static RigidbodyUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Rigidbody;
	virtual ComponentType GetComponentType() const override { return ComponentType::Rigidbody; }
	virtual void Start() override;
	virtual void Update() override;

	void SetMass(float mass);
	void SetFriction(float friction);
	void SetRestitution(float restitution);
	void SetMotionType(JPH::EMotionType motionType);
	void SetUseGravity(bool useGravity);

private:
	Rigidbody();
	virtual bool CreateBody() override;

	// 물리 속성 캐싱 (바디 생성 시 사용)
	float m_mass = 1.0f;
	float m_friction = 0.5f;
	float m_restitution = 0.0f;
	bool m_useGravity = true;
	JPH::EMotionType m_motionType = JPH::EMotionType::Dynamic;
};