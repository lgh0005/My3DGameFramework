#include "RuntimePch.h"
#include "MGFRenderPipeline.h"
#include "Managers/TypeManager.h"
#include "Rendering/RenderContext.h"
#include "Pipelines/RenderPasses/MGFGeometryPass.h"
#include "Pipelines/RenderPasses/MGFDeferredLightingPass.h"

namespace MGF3D
{
	MGFRenderPipeline::MGFRenderPipeline() = default;
	MGFRenderPipeline::~MGFRenderPipeline() = default;


	MGFRenderPipelineUPtr MGFRenderPipeline::Create()
	{
		auto pipeline = MGFRenderPipelineUPtr(new MGFRenderPipeline());
		if (!pipeline->Init()) return nullptr;
		return pipeline;
	}

	bool MGFRenderPipeline::Init()
	{
		m_geometryPass = MGFGeometryPass::Create();
		if (!m_geometryPass) return false;

		m_deferredLightingPass = MGFDeferredLightingPass::Create();
		if (!m_deferredLightingPass) return false;

		return true;
	}

	void MGFRenderPipeline::Render(RenderContext* context)
	{
		// [DEBUG]
		// 1. GBuffer 생성 패스
		m_geometryPass->Execute(context);

		// 2. 라이팅 패스
		m_deferredLightingPass->Execute(context);
	}

	/*==================================//
	//    RenderPipeline Custom Type    //
	//==================================*/
	int16 MGFRenderPipeline::s_typeIndex = -1;
	const MGFType* MGFRenderPipeline::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPipeline");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFRenderPipeline::Resize()
	{

	}
}