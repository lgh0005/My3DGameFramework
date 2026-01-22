#include "EnginePch.h"
#include "DisplayMappingEffect.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(DisplayMappingEffect)

DisplayMappingEffectUPtr DisplayMappingEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effect = DisplayMappingEffectUPtr(new DisplayMappingEffect());
	if (!effect->Init(priority, width, height)) return nullptr;
	return std::move(effect);
}

bool DisplayMappingEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	m_compositeProgram = RESOURCE.GetResource<Program>("standard_postprocess_postprocess");
	m_cameraDirtTexture = RESOURCE.GetResource<Texture>("camera_dirt");
	m_resultFBO = PostProcessFramebuffer::Create(width, height);

	m_compositeProgram->Use();
	m_compositeProgram->SetUniform("bloomBlur", 1);
	m_compositeProgram->SetUniform("cameraDirtTex", 2);

	return (m_compositeProgram && m_resultFBO);
}

bool DisplayMappingEffect::Render(RenderContext* context, Framebuffer* mainFBO, ScreenMesh* screenMesh)
{
	if (!context || !mainFBO) return false;

	// 1. 내 작업 공간(Internal FBO) 준비
	m_resultFBO->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT); // 깔끔하게 지우고 시작
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_compositeProgram->Use();

	// 2. 유니폼 설정
	m_compositeProgram->SetUniform("gamma", m_gamma);
	m_compositeProgram->SetUniform("exposure", m_exposure);
	m_compositeProgram->SetUniform("inverseScreenSize", glm::vec2(1.0f / m_width, 1.0f / m_height));

	// [Input 0] MainFBO (Scene Color - HDR)
	glActiveTexture(GL_TEXTURE0);
	mainFBO->GetColorAttachment(0)->Bind();
	m_compositeProgram->SetUniform("tex", 0);

	// [Input 1] Bloom Texture (Context에서 가져옴)
	Texture* bloomTex = context->GetTexture(RenderSlot::Bloom);
	if (bloomTex)
	{
		glActiveTexture(GL_TEXTURE1);
		bloomTex->Bind();
		m_compositeProgram->SetUniform("bloom", true);
	}
	else
	{
		m_compositeProgram->SetUniform("bloom", false);
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

	// 4. 결과를 MainFBO에 고속 복사 (Blit)
	// TODO : 고속 복사 로직 해소 필요
	Framebuffer::Blit(m_resultFBO.get(), mainFBO, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return true;
}

void DisplayMappingEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
	if (m_resultFBO) m_resultFBO->OnResize(width, height);
}