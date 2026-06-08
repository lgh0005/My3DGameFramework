#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(HDREffects)
	MGF_CLASS_PTR(LDREffects)
	MGF_CLASS_PTR(WorldSpaceEffects)

	MGF_CLASS_PTR(MGFPostProcessingPass)
	class MGFPostProcessingPass : public RenderPass
	{
	public:
		virtual ~MGFPostProcessingPass();
		static MGFPostProcessingPassUPtr Create();

		/*=================================//
		//   MGF3D RenderPass Custom Type  //
		//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		MGFPostProcessingPass();
		bool Init();

		HDREffectsUPtr m_hdrEffects{ nullptr };
		LDREffectsUPtr m_ldrEffects{ nullptr };
		WorldSpaceEffectsUPtr m_worldSpaceEffects{ nullptr };
	};
}