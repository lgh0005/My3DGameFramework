#pragma once
#include "Physics/PhysicsConfig.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Ray)
CLASS_PTR(RaycastHit)

CLASS_PTR(BPLayerInterfaceImpl)
CLASS_PTR(ObjectVsBroadPhaseLayerFilterImpl)
CLASS_PTR(ObjectLayerPairFilterImpl)
CLASS_PTR(PhysicsBodyActivationListener)
CLASS_PTR(PhysicsContactListener)

CLASS_PTR(JoltDebugRenderer)
CLASS_PTR(JoltGizmo)

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
#pragma endregion

class PhysicsManager
{
	DECLARE_SINGLE(PhysicsManager)

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