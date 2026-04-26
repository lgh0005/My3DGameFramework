#pragma once
#include "Rendering/RenderPipeline.h"

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)
	MGF_CLASS_PTR(TestRenderPass)

	MGF_CLASS_PTR(TestPipeline)
		class TestPipeline : public RenderPipeline
	{
	public:
		virtual ~TestPipeline();
		static TestPipelineUPtr Create();

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
		TestPipeline();
		bool Init();

		TestRenderPassUPtr m_testRenderPass { nullptr };
	};
}