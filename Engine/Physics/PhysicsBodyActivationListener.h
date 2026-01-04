#pragma once
#include <Jolt/Physics/Body/BodyActivationListener.h>

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