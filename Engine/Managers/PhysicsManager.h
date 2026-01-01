#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#pragma region FORWARD_DECLARATION
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;
class MyBodyActivationListener;
class MyContactListener;
#pragma endregion

class PhysicsManager
{
	DECLARE_SINGLE(PhysicsManager)

public:
	void Init();
	void Update();
	void Clear();

	JPH::BodyInterface& GetBodyInterface() const;
	const JPH::PhysicsSystem& GetPhysicsSystem() const { return *m_PhysicsSystem; }

private:

	/*=======================//
	//   Jolt Core Objects   //
	//=======================*/
	JPH::PhysicsSystem* m_PhysicsSystem			{ nullptr };
	JPH::TempAllocatorImpl* m_TempAllocator		{ nullptr }; // 메모리 할당기
	JPH::JobSystemThreadPool* m_JobSystem		{ nullptr };     // 멀티스레딩 잡 시스템

	/*================================//
	//   Jolt Configuration Objects   //
	//================================*/
	BPLayerInterfaceImpl* m_BPLayerInterface						    { nullptr };
	ObjectVsBroadPhaseLayerFilterImpl* m_ObjectVsBroadPhaseLayerFilter  { nullptr };
	ObjectLayerPairFilterImpl* m_ObjectLayerPairFilter					{ nullptr };

	/*==========================//
	//   Jolt event listeners   //
	//==========================*/
	MyBodyActivationListener* m_BodyActivationListener		{ nullptr };
	MyContactListener* m_ContactListener					{ nullptr };

/*=======================//
//   Raycaster members   //
//=======================*/
public:

private:

};