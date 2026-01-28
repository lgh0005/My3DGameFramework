#include "EnginePch.h"
#include "LensFlareEffect.h"
#include "Resources/Programs/Program.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Graphics/Framebuffers/Framebuffer.h"

DECLARE_DEFAULTS_IMPL(LensFlareEffect)

LensFlareEffectUPtr LensFlareEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effects = LensFlareEffectUPtr(new LensFlareEffect());
	if (!effects->Init(priority, width, height)) return nullptr;
	return std::move(effects);
}

bool LensFlareEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;
	m_lensFlareProgram = RESOURCE.GetResource<Program>("lens_flare");
	return m_lensFlareProgram != nullptr;
}

bool LensFlareEffect::Render(RenderContext* context, Framebuffer* src, Framebuffer* dst, ScreenMesh* scrn)
{
	if (!m_lensFlareProgram || !context || !src || !dst || !scrn) return false;

	// 1. 프레임버퍼 바인딩 및 초기화
	dst->Bind();
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_lensFlareProgram->Use();

	// 2. 텍스처 바인딩
	// [Slot 0] 현재까지의 장면 (Scene Color)
	glActiveTexture(GL_TEXTURE0);
	src->GetColorAttachment(0)->Bind();
	m_lensFlareProgram->SetUniform("uSceneTex", 0);

	// [Slot 1] 블룸 텍스처 (렌즈 플레어의 재료)
	Texture* bloomTex = context->GetTexture(RenderSlot::Bloom);
	if (bloomTex)
	{
		glActiveTexture(GL_TEXTURE1);
		bloomTex->Bind();
		m_lensFlareProgram->SetUniform("uBloomTex", 1);
	}

	// 3. 유니폼 파라미터 전송
	m_lensFlareProgram->SetUniform("uGhostCount", m_ghostCount);
	m_lensFlareProgram->SetUniform("uGhostSpacing", m_ghostSpacing);
	m_lensFlareProgram->SetUniform("uHaloRadius", m_haloRadius);
	m_lensFlareProgram->SetUniform("uFlareIntensity", m_flareIntensity);
	m_lensFlareProgram->SetUniform("uTexelSize", glm::vec2(1.0f / m_width, 1.0f / m_height));

	// 4. 그리기
	scrn->Draw();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}