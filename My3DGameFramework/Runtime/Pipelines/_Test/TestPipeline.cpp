#include "RuntimePch.h"
#include "TestPipeline.h"
#include "Managers/TypeManager.h"
#include "Managers/WindowManager.h"
#include "Rendering/RenderContext.h"
#include "Pipelines/_Test/RenderPasses/TestRenderPass.h"

namespace MGF3D
{
	TestPipeline::TestPipeline() = default;
	TestPipeline::~TestPipeline() = default;

	TestPipelineUPtr TestPipeline::Create()
	{
		auto pipeline = TestPipelineUPtr(new TestPipeline());
		if (!pipeline->Init()) return nullptr;
		return pipeline;
	}

	bool TestPipeline::Init()
	{
		m_testRenderPass = TestRenderPass::Create();
		if (!m_testRenderPass) return false;
		return true;
	}

	void TestPipeline::Render(RenderContext* context)
	{
		m_testRenderPass->Execute(context);
	}

	void TestPipeline::Resize()
	{
		// TODO : Resize Logic
	}

	/*==================================//
	//    RenderPipeline Custom Type    //
	//==================================*/
	int16 TestPipeline::s_typeIndex = -1;
	const MGFType* TestPipeline::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("TestPipeline");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}