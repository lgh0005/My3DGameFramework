#pragma once
#include "PhysicsConfig.h" 

CLASS_PTR(BPLayerInterfaceImpl)
class BPLayerInterfaceImpl final 
	: public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl();
	virtual JPH::uint GetNumBroadPhaseLayers() const override;
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif

private:
	JPH::BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
};

