#pragma once

namespace MGF3D
{
	class PhysicsManager
	{
		MGF_DECLARE_SINGLE(PhysicsManager)

	public:
		void Init();
		void Update(float deltaTime);
		void Shutdown();

		JPH::BodyInterface& GetBodyInterface();

	private:
		PhysicsSystemUPtr m_physicsSystem;
	};
}
