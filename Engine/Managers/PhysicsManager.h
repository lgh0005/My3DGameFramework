#pragma once
#include "Physics/PhysicsConfig.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(BPLayerInterfaceImpl)
CLASS_PTR(ObjectVsBroadPhaseLayerFilterImpl)
CLASS_PTR(ObjectLayerPairFilterImpl)
CLASS_PTR(PhysicsBodyActivationListener)
CLASS_PTR(PhysicsContactListener)

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

private:
	PhysicsManager();
	~PhysicsManager();

	/*=======================//
	//   Jolt Core Objects   //
	//=======================*/
	PhysicsSystemUPtr			m_PhysicsSystem;
	TempAllocatorImplUPtr		m_TempAllocator;
	JobSystemThreadPoolUPtr		m_JobSystem;

	/*================================//
	//   Jolt Configuration Objects   //
	//================================*/
	BPLayerInterfaceImplUPtr				m_BPLayerInterface;
	ObjectVsBroadPhaseLayerFilterImplUPtr	m_ObjectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImplUPtr			m_ObjectLayerPairFilter;

	/*==========================//
	//   Jolt event listeners   //
	//==========================*/
	PhysicsBodyActivationListenerUPtr	m_BodyActivationListener;
	PhysicsContactListenerUPtr			m_ContactListener;
};