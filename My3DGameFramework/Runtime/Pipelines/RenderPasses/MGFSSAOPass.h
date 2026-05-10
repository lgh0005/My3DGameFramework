#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	MGF_CLASS_PTR(MGFSSAOPass)
	class MGFSSAOPass : public RenderPass
	{
	public:
		virtual ~MGFSSAOPass();
		static MGFSSAOPassUPtr Create();

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		MGFSSAOPass();
		bool Init();
	};
}