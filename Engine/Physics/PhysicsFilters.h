#pragma once
#include "PhysicsConfig.h"

/*=======================//
//   BroadPhase Filter   //
//=======================*/
CLASS_PTR(ObjectVsBroadPhaseLayerFilterImpl)
class ObjectVsBroadPhaseLayerFilterImpl final 
	: public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide
	(
		JPH::ObjectLayer inLayer1, 
		JPH::BroadPhaseLayer inLayer2
	) const override;
};

/*=======================//
//  ObjectLayer Filter   //
//=======================*/
CLASS_PTR(ObjectLayerPairFilterImpl)
class ObjectLayerPairFilterImpl final
	: public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide
	(
		JPH::ObjectLayer inObject1, 
		JPH::ObjectLayer inObject2
	) const override;
};
