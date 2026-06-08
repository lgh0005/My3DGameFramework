#include "RuntimeModule.h"
#include "MGFPostProcessingPass.h"
#include "Managers/TypeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/EntityManager.h"
#include "Rendering/RenderContext.h"
#include "Framebuffers/GLFramebuffer2D.h"
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
		m_ldrEffects = LDREffects::Create();
		m_worldSpaceEffects = WorldSpaceEffects::Create();
		if (!m_ldrEffects || !m_worldSpaceEffects) return false;
		return true;
	}

	void MGFPostProcessingPass::Resize()
	{
		int32 width = MGF_WINDOW.GetWindowWidth();
		int32 height = MGF_WINDOW.GetWindowHeight();
		m_ldrEffects->Resize(width, height);
		m_worldSpaceEffects->Resize(width, height);
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

		glDisable(GL_DEPTH_TEST);

		// 1. World-Space Effects (Fog, Pixelization 등)
		if (m_worldSpaceEffects)
		{
			context->GetSceneBufferB()->Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			m_worldSpaceEffects->Render(context);
			context->SwapSceneBuffers();
		}

		// 2. LDR Effects (Tone Mapping, Gamma, Vignette 등)
		if (m_ldrEffects)
		{
			context->GetSceneBufferB()->Bind();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			m_ldrEffects->Render(context);
			context->SwapSceneBuffers();
		}

		context->GetSceneBuffer()->Unbind();
	}
}