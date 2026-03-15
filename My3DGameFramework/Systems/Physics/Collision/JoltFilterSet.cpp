#include "PhysicsPch.h"
#include "JoltFilterSet.h"
#include "Managers/CollisionManager.h"

namespace MGF3D
{
	JoltFilterSet::JoltFilterSet() = default;
	JoltFilterSet::~JoltFilterSet() = default;

	/*==========================================//
	//   ObjectLayerPairFilter implementation   //
	//==========================================*/
	bool JoltFilterSet::ShouldCollide(JPH::ObjectLayer inL1, JPH::ObjectLayer inL2) const
	{
		return MGF_COLLISION.ShouldCollideObjects(inL1, inL2);
	}

	/*==================================================//
	//   ObjectVsBroadPhaseLayerFilter implementation   //
	//==================================================*/
	bool JoltFilterSet::ShouldCollide(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBP) const
	{
		return MGF_COLLISION.ShouldCollideBroadPhase(inLayer, inBP);
	}

	/*==============================================//
	//   BroadPhaseLayerInterface implementations   //
	//==============================================*/
	uint32 JoltFilterSet::GetNumBroadPhaseLayers() const
	{
		return static_cast<uint32>(MGF_COLLISION.GetBPLayerCount());
	}

	JPH::BroadPhaseLayer JoltFilterSet::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
	{
		return MGF_COLLISION.GetBroadPhaseOfLayer(inLayer);
	}

	cstr JoltFilterSet::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
	{
		return cstr();
	}
}