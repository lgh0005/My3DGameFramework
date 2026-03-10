#pragma once
#include "Physics/PhysicsConfig.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Ray)
	MGF_CLASS_PTR(RaycastHit)

	MGF_CLASS_PTR(BPLayerInterfaceImpl)
	MGF_CLASS_PTR(ObjectVsBroadPhaseLayerFilterImpl)
	MGF_CLASS_PTR(ObjectLayerPairFilterImpl)
	MGF_CLASS_PTR(PhysicsBodyActivationListener)
	MGF_CLASS_PTR(PhysicsContactListener)

	MGF_CLASS_PTR(JoltDebugRenderer)
	MGF_CLASS_PTR(JoltGizmo)

	namespace JPH
	{
		class PhysicsSystem;
		class TempAllocatorImpl;
		class JobSystemThreadPool;
		class BodyInterface;
	}

	using PhysicsSystemUPtr = std::unique_ptr<JPH::PhysicsSystem>;
	using TempAllocatorImplUPtr = std::unique_ptr<JPH::TempAllocatorImpl>;
	using JobSystemThreadPoolUPtr = std::unique_ptr<JPH::JobSystemThreadPool>;

	class PhysicsManager
	{
		MGF_DECLARE_SINGLE(PhysicsManager)

	private:
		PhysicsManager();
		~PhysicsManager();

	public:
		void Init();
		void Update();
		void Clear();

		// Jolt 인터페이스 접근
		JPH::BodyInterface& GetBodyInterface() const;
		const JPH::PhysicsSystem& GetPhysicsSystem() const { return *m_PhysicsSystem; }

		/*========================//
		//   raycasting methods   //
		//========================*/
	public:
		bool Raycast
		(
			const glm::vec3& origin,
			const glm::vec3& direction,
			float maxDistance,
			RaycastHit& outHit
		);

		bool Raycast(const Ray& ray, float maxDistance, RaycastHit& outHit);

	private:


		/*=======================//
		//   debugging methods   //
		//=======================*/
	public:
		void DrawDebugData();
		JoltGizmo* GetGizmo();

	private:
		JoltGizmoUPtr m_gizmo;
		JoltDebugRendererUPtr m_debugRenderer;

		/*=======================//
		//   Jolt core members   //
		//=======================*/
	private:
		PhysicsManager();
		~PhysicsManager();

		// Jolt Core Objects
		PhysicsSystemUPtr			m_PhysicsSystem;
		TempAllocatorImplUPtr		m_TempAllocator;
		JobSystemThreadPoolUPtr		m_JobSystem;

		// Jolt Configuration Objects
		BPLayerInterfaceImplUPtr				m_BPLayerInterface;
		ObjectVsBroadPhaseLayerFilterImplUPtr	m_ObjectVsBroadPhaseLayerFilter;
		ObjectLayerPairFilterImplUPtr			m_ObjectLayerPairFilter;

		// Jolt event listeners
		PhysicsBodyActivationListenerUPtr	m_BodyActivationListener;
		PhysicsContactListenerUPtr			m_ContactListener;
	};
}
