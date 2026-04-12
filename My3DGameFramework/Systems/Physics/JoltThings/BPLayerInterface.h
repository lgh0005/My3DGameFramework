#pragma once
#include "PhysicsConfig.h" 

CLASS_PTR(BPLayerInterfaceImpl)
class BPLayerInterfaceImpl final 
	: public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl();
	virtual ~BPLayerInterfaceImpl();
	virtual JPH::uint GetNumBroadPhaseLayers() const override;
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;

private:
	JPH::BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
};

