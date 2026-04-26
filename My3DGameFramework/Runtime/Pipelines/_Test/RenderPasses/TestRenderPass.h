#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	MGF_CLASS_PTR(TestRenderPass)
	class TestRenderPass : public RenderPass
	{
	public:
		virtual ~TestRenderPass();
		static TestRenderPassUPtr Create();

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		TestRenderPass();
		bool Init();

		// TODO : 기본 렌더링 프로그램
		GraphicsProgramPtr m_program;
	};
}