#include "EnginePch.h"
#include "Physics/BPLayerInterface.h"
using namespace JPH;

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
	m_objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
	m_objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}
BPLayerInterfaceImpl::~BPLayerInterfaceImpl() = default;

uint32 BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
	if (inLayer >= Layers::NUM_LAYERS) return BroadPhaseLayers::NON_MOVING;
	return m_objectToBroadPhase[inLayer];
}

const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const
{
	switch ((BroadPhaseLayer::Type)inLayer)
	{
	case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
	case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:     return "MOVING";
	default:                                                  return "INVALID";
	}
}