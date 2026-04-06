#include "EnginePch.h"
#include "Physics/PhysicsFilters.h"
using namespace JPH;

/*===============================//
//   BroadPhase Filter Methods   //
//===============================*/
DECLARE_DEFAULTS_IMPL(ObjectVsBroadPhaseLayerFilterImpl)

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide
(
	ObjectLayer inLayer1, 
	BroadPhaseLayer inLayer2
) const
{
	switch (inLayer1)
	{
	case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
	case Layers::MOVING:     return true;
	default:                 return false;
	}
}

/*==============================//
//  ObjectLayer Filter Methods  //
//==============================*/
DECLARE_DEFAULTS_IMPL(ObjectLayerPairFilterImpl)

bool ObjectLayerPairFilterImpl::ShouldCollide
(
	ObjectLayer inObject1, 
	ObjectLayer inObject2
) const
{
	// TODO : 이후에 충돌 방식 구현을 확실히 해둘 필요가 있음
	switch (inObject1)
	{
	case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // ex) 벽끼리는 충돌 X
	case Layers::MOVING:     return true;
	default:                 return false;
	}
}