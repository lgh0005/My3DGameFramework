#pragma once
#include "Rendering/RenderPipeline.h"

namespace MGF3D
{
	// render pipeline resources
	MGF_CLASS_PTR(RenderContext)

	// main render passes
	MGF_CLASS_PTR(MGFGeometryPass)
	MGF_CLASS_PTR(MGFDeferredLightingPass)

	// MGF3D main render pipeline class
	MGF_CLASS_PTR(MGFRenderPipeline)
	class MGFRenderPipeline : public RenderPipeline
	{
	public:
		virtual ~MGFRenderPipeline();
		static MGFRenderPipelineUPtr Create();

	/*=====================================//
	//   MGF3D RenderPipeline Custom Type  //
	//=====================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Render(RenderContext* context) override;
		virtual void Resize() override;

	private:
		MGFRenderPipeline();
		bool Init();

		MGFGeometryPassUPtr m_geometryPass;
		MGFDeferredLightingPassUPtr m_deferredLightingPass;
	};
}