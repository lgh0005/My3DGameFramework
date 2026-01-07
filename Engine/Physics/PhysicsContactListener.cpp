#include "EnginePch.h"
#include "Physics/PhysicsContactListener.h"
#include "Scene/GameObject.h"
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/PhysicsSystem.h>
using namespace JPH;

PhysicsContactListener::PhysicsContactListener()
{
	m_eventQueue.reserve(1024);
}
PhysicsContactListener::~PhysicsContactListener() = default;

ValidateResult PhysicsContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
	// 모든 충돌 허용 (특정 조건에서 무시하려면 여기서 처리)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void PhysicsContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	ScopedLock lock(m_mutex);
	m_eventQueue.push_back({ CollisionType::Enter, inBody1.GetUserData(), inBody2.GetUserData() });
}

void PhysicsContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	if (inBody1.IsStatic() || inBody2.IsStatic()) return;
	ScopedLock lock(m_mutex);
	m_eventQueue.push_back({ CollisionType::Stay, inBody1.GetUserData(), inBody2.GetUserData() });
}

void PhysicsContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	// 1. BodyInterface 가져오기 가져오기
	const PhysicsSystem& system = PHYSICS.GetPhysicsSystem();
	const BodyInterface& bodyInterface = system.GetBodyInterfaceNoLock();
	uint64 userData1 = bodyInterface.GetUserData(inSubShapePair.GetBody1ID());
	uint64 userData2 = bodyInterface.GetUserData(inSubShapePair.GetBody2ID());

	// 2. 둘 다 유효한 정보가 있을 때만 이벤트 큐에 등록
	if (userData1 != 0 && userData2 != 0)
	{
		ScopedLock lock(m_mutex);
		m_eventQueue.push_back({ CollisionType::Exit, userData1, userData2 });
	}
}

void PhysicsContactListener::DispatchEvents()
{
	// 1. 큐 스왑
	std::vector<CollisionEvent> dispatchQueue;
	{
		ScopedLock lock(m_mutex);
		if (m_eventQueue.empty()) return;
		dispatchQueue.swap(m_eventQueue);
	}

	// 2. 처리 (락 없이 자유롭게 실행)
	for (const auto& evt : dispatchQueue)
	{
		GameObject* obj1 = reinterpret_cast<GameObject*>(evt.body1UserData);
		GameObject* obj2 = reinterpret_cast<GameObject*>(evt.body2UserData);
		if (!obj1 || !obj2) continue;

		// TODO : 이쪽은 GameObject가 아니라 이제 Script 단에서 구현할
		// 충돌과 관련된 메서드들을 여기서 이제 처리할 예정
		/*switch (evt.type)
		{
		case CollisionType::Enter: LOG_INFO("This event is Enter {}", (int)evt.type); break;
		case CollisionType::Stay: LOG_INFO("This event is Stay {}", (int)evt.type); break;
		case CollisionType::Exit: LOG_INFO("This event is Exit {}", (int)evt.type); break;
		}*/
	}
}