#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

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


	};
}