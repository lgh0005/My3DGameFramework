#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(PhysicsBodyActivationListener)
	class PhysicsBodyActivationListener final : public JPH::BodyActivationListener
	{
	public:
		PhysicsBodyActivationListener();
		virtual ~PhysicsBodyActivationListener() override;

	public:
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;
		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;
	};
}