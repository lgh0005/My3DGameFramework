#pragma once

namespace MGF3D
{
	class PhysicsManager
	{
		MGF_DECLARE_SINGLE(PhysicsManager)

	private:
		PhysicsManager();
		~PhysicsManager();

	public:
		void Init();
		void Update(float deltaTime);
		void Shutdown();

	/*========================//
	//   Jolt API providers   //
	//========================*/
	public:
		JPH::BodyInterface& GetBodyInterface();
		const JPH::NarrowPhaseQuery& GetNarrowPhaseQuery() const;
		Ptr<JPH::PhysicsSystem> GetSystem() const;

	private:
		PhysicsSystemUPtr m_physicsSystem;

	/*==========================//
	//   Global physics rules   //
	//==========================*/
	public:
		void SetGravity(const vec3& inGravity);
		vec3 GetGravity() const;

		void SetAutoSimulation(bool inAuto);
		bool IsAutoSimulation() const;

	private:
		vec3 m_gravity			{ 0.0f, -9.81f, 0.0f };
		bool m_autoSimulation	{ true };
	};
}
