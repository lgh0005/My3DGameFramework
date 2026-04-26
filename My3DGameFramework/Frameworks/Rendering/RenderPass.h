#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)

	MGF_CLASS_PTR(RenderPass)
	class RenderPass
	{
	public:
		virtual ~RenderPass();
		virtual void Execute(RenderContext& context) = 0;

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	protected:
		RenderPass();
	};
}