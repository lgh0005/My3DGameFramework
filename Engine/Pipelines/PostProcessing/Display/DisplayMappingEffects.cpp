#include "EnginePch.h"
#include "DisplayMappingEffects.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(DisplayMappingEffects)

DisplayMappingEffectsUPtr DisplayMappingEffects::Create(int32 priority, int32 width, int32 height)
{
	auto effect = DisplayMappingEffectsUPtr(new DisplayMappingEffects());
	if (!effect->Init(priority, width, height)) return nullptr;
	return std::move(effect);
}

bool DisplayMappingEffects::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	m_compositeProgram = RESOURCE.GetResource<Program>("standard_postprocess_postprocess");
	m_cameraDirtTexture = RESOURCE.GetResource<Texture>("camera_dirt");
	if (!m_compositeProgram) return false;

	m_compositeProgram->Use();
	m_compositeProgram->SetUniform("bloomBlur", 1);
	m_compositeProgram->SetUniform("cameraDirtTex", 2);

	return true;
}

bool DisplayMappingEffects::Render(RenderContext* context, Framebuffer* srcFBO, Framebuffer* dstFBO, ScreenMesh* screenMesh)
{
	if (!context || !srcFBO || !dstFBO) return false;

	// 1. 내 작업 공간(Internal FBO) 준비
	dstFBO->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	m_compositeProgram->Use();

	// 2. 유니폼 설정
	m_compositeProgram->SetUniform("gamma", m_gamma);
	m_compositeProgram->SetUniform("exposure", m_exposure);
	m_compositeProgram->SetUniform("inverseScreenSize", glm::vec2(1.0f / m_width, 1.0f / m_height));
	m_compositeProgram->SetUniform("toneMappingMode", (int32)m_toneMappingMode);

	// 3. 추가 효과 유니폼 설정
	m_compositeProgram->SetUniform("time", (float)TIME.GetTime());
	m_compositeProgram->SetUniform("useCRT", m_useCRT);
	m_compositeProgram->SetUniform("distortionK", m_distortionK);
	m_compositeProgram->SetUniform("scanlineIntensity", m_scanlineIntensity);
	m_compositeProgram->SetUniform("phosphorIntensity", m_phosphorIntensity);
	m_compositeProgram->SetUniform("flickerIntensity", m_flickerIntensity);
	m_compositeProgram->SetUniform("chromaticAmount", m_chromaticAmount);
	m_compositeProgram->SetUniform("vignetteIntensity", m_vignetteIntensity);
	m_compositeProgram->SetUniform("grainAmount", m_grainAmount);
	m_compositeProgram->SetUniform("scanlineIntensity", m_scanlineIntensity);
	m_compositeProgram->SetUniform("sharpenAmount", m_sharpenAmount);

	// [Input 0] MainFBO (Scene Color - HDR)
	glActiveTexture(GL_TEXTURE0);
	srcFBO->GetColorAttachment(0)->Bind();
	m_compositeProgram->SetUniform("tex", 0);

	// [Input 1] Bloom Texture (Context에서 가져옴)
	Texture* bloomTex = context->GetTexture(RenderSlot::Bloom);
	if (bloomTex)
	{
		glActiveTexture(GL_TEXTURE1);
		bloomTex->Bind();
		m_compositeProgram->SetUniform("bloom", true);
		m_compositeProgram->SetUniform("bloomStrength", m_bloomStrength);
	}
	else
	{
		m_compositeProgram->SetUniform("bloom", false);
		m_compositeProgram->SetUniform("bloomStrength", 0.0f);
	}

	// [Input 2] Dirt Texture (Optional)
	if (m_cameraDirtTexture)
	{
		glActiveTexture(GL_TEXTURE2);
		m_cameraDirtTexture->Bind();
		m_compositeProgram->SetUniform("dirtAmbient", m_cameraDirtAmbient);
		m_compositeProgram->SetUniform("dirtIntensity", m_cameraDirtIntensity);
	}

	// 3. 합성 그리기 (Internal FBO에 그려짐)
	screenMesh->Draw();

	return true;
}

void DisplayMappingEffects::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
}