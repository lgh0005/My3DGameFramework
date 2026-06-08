#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	MGF_CLASS_PTR(MGFGeometryPass)
	class MGFGeometryPass : public RenderPass
	{
	public:
		virtual ~MGFGeometryPass();
		static MGFGeometryPassUPtr Create();

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;
		virtual void Resize() override;

	private:
		MGFGeometryPass();
		bool Init();

		GraphicsProgramPtr m_geometryStaticDrawProgram;
		GraphicsProgramPtr m_geometrySkinnedDrawProgram;
	};
}