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
	// TODO : PhysicsManager가 가지는 이유는,
	// 컴포넌트로 들고 있으면 "누가 어느 Ray에 맞았는 지"에 대한 추적이 매우 어려워지고 비효율
	// 싱글톤 매니저가 이를 가지고 "이 Raw에 맞은 대상을 찾는" 식을 구현하면 코드 작성이 용이

private:

};