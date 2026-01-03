#include "EnginePch.h"
#include "Physics/PhysicsBodyActivationListener.h"
using namespace JPH;

void PhysicsBodyActivationListener::OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData)
{
	// TODO : 물체가 정지해서 엔진이 계산을 멈출 때 호출됩니다.
	LOG_INFO("Body Activated: {}", inBodyID.GetIndex());
}

void PhysicsBodyActivationListener::OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData)
{
	// TODO : 물체가 충돌하거나 힘을 받아서 다시 움직이기 시작할 때 호출됩니다.
	LOG_INFO("Body Deactivated: {}", inBodyID.GetIndex());
}