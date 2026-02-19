#pragma once
#include <Jolt/Physics/Collision/ContactListener.h>

enum class CollisionType : uint8
{
	Enter,
	Stay,
	Exit
};

struct CollisionEvent
{
	CollisionType type;
	uint64 body1UserData;
	uint64 body2UserData;
};

CLASS_PTR(PhysicsContactListener)
class PhysicsContactListener final : public JPH::ContactListener
{
public:
	PhysicsContactListener();
	virtual ~PhysicsContactListener();

	// 이벤트 디스패쳐
	void DispatchEvents();

/*==========================//
//  event dispatch methods  //
//==========================*/
private:
	ThreadMutex m_mutex;
	std::vector<CollisionEvent> m_collisionEventQueue;
	void DispatchTriggerEvents(const CollisionEvent& evt, const GameObject* obj1, const GameObject* obj2);
	void DispatchCollideEvents(const CollisionEvent& evt, const GameObject* obj1, const GameObject* obj2);

/*===========================//
//  collision check methods  //
//===========================*/
private:
	// 1. 충돌 검증 (여기서 false 리턴 시 충돌 무시)
	virtual JPH::ValidateResult OnContactValidate
	(
		const JPH::Body& inBody1,
		const JPH::Body& inBody2,
		JPH::RVec3Arg inBaseOffset,
		const JPH::CollideShapeResult& inCollisionResult
	) override;

	// 2. 충돌 시작 (Enter)
	virtual void OnContactAdded
	(
		const JPH::Body& inBody1,
		const JPH::Body& inBody2,
		const JPH::ContactManifold& inManifold,
		JPH::ContactSettings& ioSettings
	) override;

	// 3. 충돌 유지 (Stay)
	virtual void OnContactPersisted
	(
		const JPH::Body& inBody1,
		const JPH::Body& inBody2,
		const JPH::ContactManifold& inManifold,
		JPH::ContactSettings& ioSettings
	) override;

	// 4. 충돌 종료 (Exit)
	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};