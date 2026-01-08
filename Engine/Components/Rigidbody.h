#pragma once
#include "PhysicsComponent.h"
#include "Physics/RigidbodyConstraint.h"
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

private:
	virtual bool CreateBody() override;
	void ApplyMotionProperties();

/*==================================//
//   default rigidbody properties   //
//==================================*/
public:
	void SetMass(float mass);
	void SetFriction(float friction);
	void SetRestitution(float restitution);
	void SetMotionType(JPH::EMotionType motionType);
	void SetUseGravity(bool useGravity);

/*===========================//
//   rigidbody constraints   //
//===========================*/
public:
	void SetConstraints(const RigidbodyConstraint& constraints);
	const RigidbodyConstraint& GetConstraints() const { return m_constraints; }
	void FreezePosition(bool x, bool y, bool z);
	void FreezeRotation(bool x, bool y, bool z);

/*===========================//
//   force & velocity        //
//===========================*/
public:
	void AddForce(const glm::vec3& force);
	void AddImpulse(const glm::vec3& impulse);

	glm::vec3 GetLinearVelocity() const;
	void SetLinearVelocity(const glm::vec3& velocity);

	glm::vec3 GetAngularVelocity() const;
	void SetAngularVelocity(const glm::vec3& velocity);

private:
	Rigidbody();

	// 물리 속성 캐싱 (바디 생성 시 사용)
	float m_mass = 1.0f;
	float m_friction = 0.5f;
	float m_restitution = 0.0f;
	bool m_useGravity = true;
	JPH::EMotionType m_motionType = JPH::EMotionType::Dynamic;

	RigidbodyConstraint m_constraints;
	bool m_isMotionPropsDirty = false;
};