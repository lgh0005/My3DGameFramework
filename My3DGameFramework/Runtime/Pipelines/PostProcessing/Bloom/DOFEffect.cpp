#include "EnginePch.h"
#include "DOFEffect.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Graphics/Framebuffers/Framebuffer.h"

DECLARE_DEFAULTS_IMPL(DOFEffect)

DOFEffectUPtr DOFEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effects = DOFEffectUPtr(new DOFEffect());
	if (!effects->Init(priority, width, height)) return nullptr;
	return std::move(effects);
}

bool DOFEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;
	m_DepthOfFieldProgram = RESOURCE.Add<GraphicsProgram>
	(
		"dof_effect",
		"@BuiltInAsset/Shaders/PostProcessing/PostProcess_Depth_Of_View_Effect.vert",
		"@BuiltInAsset/Shaders/PostProcessing/PostProcess_Depth_Of_View_Effect.frag"
	);
	return m_DepthOfFieldProgram != nullptr;
}

bool DOFEffect::Render(RenderContext* context, Framebuffer* src, Framebuffer* dst, ScreenMesh* scrn)
{
	dst->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_DepthOfFieldProgram->Use();

	// 1. Scene Color 바인딩
	glActiveTexture(GL_TEXTURE0);
	src->GetColorAttachment(0)->Bind();
	m_DepthOfFieldProgram->SetUniform("uSceneTex", 0);

	// 2. Depth 텍스처 바인딩 (srcFBO에 Depth가 포함되어 있어야 함)
	Texture* depthTex = context->GetTexture(RenderSlot::GVelocity);
	if (depthTex)
	{
		glActiveTexture(GL_TEXTURE1);
		depthTex->Bind();
		m_DepthOfFieldProgram->SetUniform("uDepthTex", 1);
	}

	// 3. 유니폼 설정
	m_DepthOfFieldProgram->SetUniform("uFocusDistance", m_focusDistance);
	m_DepthOfFieldProgram->SetUniform("uFocusRange", m_focusRange);
	m_DepthOfFieldProgram->SetUniform("uBokehRadius", m_bokehRadius);
	m_DepthOfFieldProgram->SetUniform("uTexelSize", glm::vec2(1.0f / m_width, 1.0f / m_height));

	scrn->Draw();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void DOFEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
}
