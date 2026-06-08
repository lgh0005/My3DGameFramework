#include "RuntimePch.h"
#include "MGFRenderPipeline.h"
#include "Managers/TypeManager.h"
#include "Rendering/RenderContext.h"

#pragma region MAIN_RNEDER_PASSES
#include "Pipelines/RenderPasses/MGFShadowPass.h"
#include "Pipelines/RenderPasses/MGFSSAOPass.h"
#include "Pipelines/RenderPasses/MGFGeometryPass.h"
#include "Pipelines/RenderPasses/MGFDeferredLightingPass.h"
#include "Pipelines/RenderPasses/MGFSkyboxPass.h"
#include "Pipelines/RenderPasses/MGFPostProcessingPass.h"
#pragma endregion

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
		m_shadowPass = MGFShadowPass::Create();
		if (!m_shadowPass) return false;

		m_ssaoPass = MGFSSAOPass::Create();
		if (!m_ssaoPass) return false;

		m_geometryPass = MGFGeometryPass::Create();
		if (!m_geometryPass) return false;

		m_deferredLightingPass = MGFDeferredLightingPass::Create();
		if (!m_deferredLightingPass) return false;

		m_skyboxPass = MGFSkyboxPass::Create();
		if (!m_deferredLightingPass) return false;

		m_postProcessPass = MGFPostProcessingPass::Create();
		if (!m_deferredLightingPass) return false;

		return true;
	}

	void MGFRenderPipeline::Render(RenderContext* context)
	{
		// 1. Shadow 패스
		m_shadowPass->Execute(context);

		// 2. GBuffer 생성 패스
		m_geometryPass->Execute(context);

		// 3. SSSAO 패스
		m_ssaoPass->Execute(context);

		// 4. 라이팅 패스
		m_deferredLightingPass->Execute(context);

		// 5. 스카이 패스
		m_skyboxPass->Execute(context);

		// 6. 포스트-프로세스 패스
		m_postProcessPass->Execute(context);
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
}