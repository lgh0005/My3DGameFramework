#include "RuntimeModule.h"
#include "MGFPostProcessingPass.h"
#include "Managers/TypeManager.h"
#include "Rendering/RenderContext.h"
#include "PostProcessing/Effects/HDREffects.h"
#include "PostProcessing/Effects/LDREffects.h"
#include "PostProcessing/Effects/WorldSpaceEffects.h"

namespace MGF3D
{
	MGFPostProcessingPass::MGFPostProcessingPass() = default;
	MGFPostProcessingPass::~MGFPostProcessingPass() = default;

	MGFPostProcessingPassUPtr MGFPostProcessingPass::Create()
	{
		auto renderPass = MGFPostProcessingPassUPtr(new MGFPostProcessingPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFPostProcessingPass::Init()
	{
		m_hdrEffects = HDREffects::Create();
		m_ldrEffects = LDREffects::Create();
		m_worldSpaceEffects = WorldSpaceEffects::Create();
		if (!m_hdrEffects || !m_ldrEffects || !m_worldSpaceEffects) return false;
		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFPostProcessingPass::s_typeIndex = -1;
	const MGFType* MGFPostProcessingPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFPostProcessingPass::Execute(RenderContext* context)
	{
		if (!context) return;

		// 1. World-Space Effects (Fog, Pixelization 등)
		if (m_worldSpaceEffects)
		{
			context->GetSceneBufferB()->Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			m_worldSpaceEffects->Render(context);
			context->SwapSceneBuffers();
		}

		//// 2. HDR Effects (Bloom, Lens Flare, DoF 등)
		//if (m_hdrEffects)
		//{
		//	context->GetSceneBufferB()->Bind();
		//	glClear(GL_COLOR_BUFFER_BIT);
		//	m_hdrEffects->Render(context);
		//	context->SwapSceneBuffers();
		//}

		// 3. LDR Effects (Tone Mapping, Gamma, Vignette, Screen Dirt 등)
		if (m_ldrEffects)
		{
			context->GetSceneBufferB()->Bind();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			m_ldrEffects->Render(context);
			context->SwapSceneBuffers();
		}

		context->GetSceneBuffer()->Unbind();
	}
}