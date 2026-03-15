#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(JoltFilterSet)
	class JoltFilterSet final :
		public JPH::ObjectLayerPairFilter,
		public JPH::ObjectVsBroadPhaseLayerFilter,
		public JPH::BroadPhaseLayerInterface
	{
	public:
		JoltFilterSet();
		virtual ~JoltFilterSet();

	/*==========================================//
	//   ObjectLayerPairFilter implementation   //
	//==========================================*/
	public:
		bool ShouldCollide(JPH::ObjectLayer inL1, JPH::ObjectLayer inL2) const override;

	/*==================================================//
	//   ObjectVsBroadPhaseLayerFilter implementation   //
	//==================================================*/
	public:
		bool ShouldCollide(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBP) const override;

	/*==============================================//
	//   BroadPhaseLayerInterface implementations   //
	//==============================================*/
	public:
		uint32 GetNumBroadPhaseLayers() const override;
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;
		cstr GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
	};
}