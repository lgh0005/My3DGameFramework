#include "EnginePch.h"
#include "Physics/PhysicsContactListener.h"
#include "Object/GameObject.h"
#include "Components/Collider.h"
#include "Components/Script.h"
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/PhysicsSystem.h>
using namespace JPH;

PhysicsContactListener::PhysicsContactListener()
{
	m_collisionEventQueue.reserve(1024);
}
PhysicsContactListener::~PhysicsContactListener() = default;

/*===========================//
//  collision check methods  //
//===========================*/
ValidateResult PhysicsContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
	// 모든 충돌 허용 (특정 조건에서 무시하려면 여기서 처리)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void PhysicsContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	ScopedLock lock(m_mutex);
	m_collisionEventQueue.push_back({ CollisionType::Enter, inBody1.GetUserData(), inBody2.GetUserData() });
}

void PhysicsContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	if (inBody1.IsStatic() || inBody2.IsStatic()) return;
	ScopedLock lock(m_mutex);
	m_collisionEventQueue.push_back({ CollisionType::Stay, inBody1.GetUserData(), inBody2.GetUserData() });
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
		m_collisionEventQueue.push_back({ CollisionType::Exit, userData1, userData2 });
	}
}

/*==========================//
//  event dispatch methods  //
//==========================*/
void PhysicsContactListener::DispatchEvents()
{
	// 1. 큐 스왑
	std::vector<CollisionEvent> dispatchQueue;
	{
		ScopedLock lock(m_mutex);
		if (m_collisionEventQueue.empty()) return;
		dispatchQueue.swap(m_collisionEventQueue);
	}

	// 2. 처리 (락 없이 자유롭게 실행)
	for (const auto& evt : dispatchQueue)
	{
		GameObject* obj1 = reinterpret_cast<GameObject*>(evt.body1UserData);
		GameObject* obj2 = reinterpret_cast<GameObject*>(evt.body2UserData);
		if (!obj1 || !obj2) continue;

		// 3. Trigger(Sensor) 여부 판별
		Collider* col1 = obj1->GetComponent<Collider>();
		Collider* col2 = obj2->GetComponent<Collider>();

		bool isTrigger = false;
		if (col1 && col1->IsTrigger()) isTrigger = true;
		if (col2 && col2->IsTrigger()) isTrigger = true;

		// 4. 로그 출력 (동작 확인용)
		if (isTrigger) DispatchTriggerEvents(evt, obj1, obj2);
		else DispatchCollideEvents(evt, obj1, obj2);
	}
}

void PhysicsContactListener::DispatchTriggerEvents(const CollisionEvent& evt, const GameObject* obj1, const GameObject* obj2)
{
	Collider* col1 = obj1->GetComponent<Collider>();
	Collider* col2 = obj2->GetComponent<Collider>();
	if (!col1 || !col1->IsActive() || !col2 || !col2->IsActive()) return;

	Script* script1 = obj1->GetComponent<Script>();
	Script* script2 = obj2->GetComponent<Script>();
	if (!script1 && !script2) return;

	// obj1 상호작용
	if (script1 && script1->IsActive())
	{
		switch (evt.type)
		{
		case CollisionType::Enter: script1->OnTriggerEnter(col2); break;
		case CollisionType::Stay:  script1->OnTriggerStay(col2);  break;
		case CollisionType::Exit:  script1->OnTriggerExit(col2);  break;
		}
	}

	// obj2 상호작용
	if (script2 && script2->IsActive())
	{
		switch (evt.type)
		{
		case CollisionType::Enter: script2->OnTriggerEnter(col1); break;
		case CollisionType::Stay:  script2->OnTriggerStay(col1);  break;
		case CollisionType::Exit:  script2->OnTriggerExit(col1);  break;
		}
	}
}

void PhysicsContactListener::DispatchCollideEvents(const CollisionEvent& evt, const GameObject* obj1, const GameObject* obj2)
{
	Collider* col1 = obj1->GetComponent<Collider>();
	Collider* col2 = obj2->GetComponent<Collider>();
	if (!col1 || !col1->IsActive() || !col2 || !col2->IsActive()) return;

	Script* script1 = obj1->GetComponent<Script>();
	Script* script2 = obj2->GetComponent<Script>();
	if (!script1 && !script2) return;

	if (script1 && script1->IsActive())
	{
		switch (evt.type)
		{
		case CollisionType::Enter: script1->OnCollisionEnter(col2); break;
		case CollisionType::Stay:  script1->OnCollisionStay(col2);  break;
		case CollisionType::Exit:  script1->OnCollisionExit(col2);  break;
		}
	}

	if (script2 && script2->IsActive())
	{
		switch (evt.type)
		{
		case CollisionType::Enter: script2->OnCollisionEnter(col1); break;
		case CollisionType::Stay:  script2->OnCollisionStay(col1);  break;
		case CollisionType::Exit:  script2->OnCollisionExit(col1);  break;
		}
	}
}