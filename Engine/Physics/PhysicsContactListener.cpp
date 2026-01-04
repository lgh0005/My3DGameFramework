#include "EnginePch.h"
#include "Physics/PhysicsContactListener.h"
using namespace JPH;

DECLARE_DEFAULTS_IMPL(PhysicsContactListener)

ValidateResult PhysicsContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
	// 모든 충돌 허용 (특정 조건에서 무시하려면 여기서 처리)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void PhysicsContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	// TODO : 스크립트에서 OnCollisionEnter에 대한 로직 실행 부분
	LOG_INFO("Collision Enter");
}

void PhysicsContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	// TODO : 스크립트에서 OnCollisionStay에 대한 로직 실행 부분
	LOG_INFO("Collision Stay");
}

void PhysicsContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	// TODO : 스크립트에서 OnCollisionExit에 대한 로직 실행 부분
	LOG_INFO("Collision Exit");
}