#pragma once
#include <Jolt/Physics/Body/BodyActivationListener.h>

// TODO : 이쪽도 보면 
// Rigidbody 또는 Collider가 붙은 것이라면
// 이쪽에 대한 이벤트 처리가 필요할 수도 있음
// 또는 cpp에 적은 TODO 이벤트에 따른 적절한 이벤트 처리가
// 필요할 수 있음. 지금 당장에서는 필요한 것을 생각해보면,
// 아마 SetActive, OnEnable 등이 해당될 것 같음.

CLASS_PTR(PhysicsBodyActivationListener)
class PhysicsBodyActivationListener final
	: public JPH::BodyActivationListener
{
public:
	PhysicsBodyActivationListener();
	virtual ~PhysicsBodyActivationListener();

	virtual void OnBodyActivated
	(
		const JPH::BodyID& inBodyID, 
		uint64 inBodyUserData
	) override;

	virtual void OnBodyDeactivated
	(
		const JPH::BodyID& inBodyID, 
		uint64 inBodyUserData
	) override;
};