#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	MGF_CLASS_PTR(TestLightPass)
	class TestLightPass : public RenderPass
	{
	public:
		virtual ~TestLightPass();
		static TestLightPassUPtr Create();

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		TestLightPass();
		bool Init();

		GraphicsProgramPtr m_program;
	};
}