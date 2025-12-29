#include "EnginePch.h"
#include "Misc/PhysicsHelper.h"

/*==========================//
//   BPLayerInterfaceImpl   //
//==========================*/
BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

uint32 BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
    return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const
{
    switch ((BroadPhaseLayer::Type)inLayer)
    {
    case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
    case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:     return "MOVING";
    default:                                                  return "INVALID";
    }
}
#endif

/*=======================================//
//   ObjectVsBroadPhaseLayerFilterImpl   //
//=======================================*/
bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
    switch (inLayer1)
    {
    case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
        return true;
    default:
        return false;
    }
}

/*===============================//
//   ObjectLayerPairFilterImpl   //
//===============================*/
bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
    switch (inObject1)
    {
    case Layers::NON_MOVING:
        return inObject2 == Layers::MOVING; // 정적 vs 정적 충돌 X
    case Layers::MOVING:
        return true; // 동적 물체는 다 충돌
    default:
        return false;
    }
}

/*==============================//
//   MyBodyActivationListener   //
//==============================*/
void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
{ 
    /* 로그 */ 
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
{
    /* 로그 */ 
}

JPH::ValidateResult MyContactListener::OnContactValidate
(
    const JPH::Body& inBody1, const JPH::Body& inBody2,
    JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult
)
{
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded
(
    const JPH::Body& inBody1, const JPH::Body& inBody2, 
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings
)
{
    /* 충돌 시작 */ 
}

void MyContactListener::OnContactPersisted
(
    const JPH::Body& inBody1, const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings
) 
{
    /* 충돌 중 */ 
}

void MyContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
    /* 충돌 끝 */ 
}

/*======================//
//   PhysicsCallbacks   //
//======================*/
void PhysicsCallbacks::TraceImpl(const char* inFMT, ...)
{
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);
    LOG_INFO("Jolt: {}", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
bool PhysicsCallbacks::AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32 inLine)
{
    LOG_ERROR("Jolt Assert: {}:{}: ({}) {}", inFile, inLine, inExpression, inMessage ? inMessage : "");
    return true; // Breakpoint
}
#endif