#include "PhysicsPch.h"
#include "ActiveListener.h"

namespace MGF3D
{
	PhysicsBodyActivationListener::PhysicsBodyActivationListener() = default;
	PhysicsBodyActivationListener::~PhysicsBodyActivationListener() = default;

	void PhysicsBodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
	{
		// INFO : 물체가 정지해서 엔진이 계산을 멈출 때 호출됩니다.
		MGF_LOG_INFO("Body Activated: {}", inBodyID.GetIndex());
	}

	void PhysicsBodyActivationListener::OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData)
	{
		// INFO : 물체가 충돌하거나 힘을 받아서 다시 움직이기 시작할 때 호출됩니다.
		MGF_LOG_INFO("Body Deactivated: {}", inBodyID.GetIndex());
	}
}
