#include "EnginePch.h"
#include "KawaseBloomEffect.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Framebuffers/EffectFramebuffer.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(KawaseBloomEffect)

KawaseBloomEffectUPtr KawaseBloomEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effect = KawaseBloomEffectUPtr(new KawaseBloomEffect());
	if (!effect->Init(priority, width, height)) return nullptr;
	return std::move(effect);
}

bool KawaseBloomEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	m_bloomProgram = RESOURCE.GetResource<Program>("universal_postprocess_blur");
	m_bloomFBO = EffectFramebuffer::CreateEmpty();
	if (!m_bloomProgram || !m_bloomFBO) return false;

	CreateBloomMips(width, height);

	return true;
}

bool KawaseBloomEffect::Render(RenderContext* context, Framebuffer* mainFBO, ScreenMesh* screenMesh)
{
	// 0. 유효성 검사
	if (!context || !mainFBO || m_bloomMips.empty()) return false;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_bloomFBO->Bind();
	m_bloomProgram->Use();

	/*===================================================//
	//  Prefilter & First Downsample (Source -> Mip[0])  //
	//===================================================*/
	m_bloomProgram->SetUniform("threshold", m_threshold);
	m_bloomProgram->SetUniform("mode", 0); // Prefilter Mode

	// Target: Mip[0]
	m_bloomFBO->AttachTextureDirect(0, m_bloomMips[0].texture->Get());
	glViewport(0, 0, m_bloomMips[0].width, m_bloomMips[0].height);
	glClear(GL_COLOR_BUFFER_BIT);

	// Source: Main Scene (HDR)
	glActiveTexture(GL_TEXTURE0);
	mainFBO->GetColorAttachment(0)->Bind();
	m_bloomProgram->SetUniform("mainTexture", 0);

	glm::vec2 texelSize = { 1.0f / mainFBO->GetWidth(), 1.0f / mainFBO->GetHeight() };
	m_bloomProgram->SetUniform("texelSize", texelSize);

	screenMesh->Draw();

	/*=========================================//
	//  Downsample Chain (Mip[i-1] -> Mip[i])  //
	//=========================================*/
	m_bloomProgram->SetUniform("mode", 1);
	for (usize i = 1; i < m_bloomMips.size(); i++)
	{
		// Target: Current Mip
		m_bloomFBO->AttachTextureDirect(0, m_bloomMips[i].texture->Get());
		glViewport(0, 0, m_bloomMips[i].width, m_bloomMips[i].height);
		glClear(GL_COLOR_BUFFER_BIT);

		// Source: Previous Mip
		glActiveTexture(GL_TEXTURE0);
		m_bloomMips[i - 1].texture->Bind();
		m_bloomProgram->SetUniform("mainTexture", 0);

		texelSize = { 1.0f / m_bloomMips[i - 1].width, 1.0f / m_bloomMips[i - 1].height };
		m_bloomProgram->SetUniform("texelSize", texelSize);

		screenMesh->Draw();
	}

	/*=======================================//
	//  Upsample Chain (Mip[i] -> Mip[i-1])  //
	//=======================================*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE); // Additive Blending
	m_bloomProgram->SetUniform("mode", 2); // Upsample Mode

	for (usize i = m_bloomMips.size() - 1; i > 0; i--)
	{
		// Target: Upper Mip (이미 그려진 것 위에 덧칠)
		m_bloomFBO->AttachTextureDirect(0, m_bloomMips[i - 1].texture->Get());
		glViewport(0, 0, m_bloomMips[i - 1].width, m_bloomMips[i - 1].height);

		// Source: Current Mip
		glActiveTexture(GL_TEXTURE0);
		m_bloomMips[i].texture->Bind();
		m_bloomProgram->SetUniform("mainTexture", 0);

		texelSize = { 1.0f / m_bloomMips[i].width, 1.0f / m_bloomMips[i].height };
		m_bloomProgram->SetUniform("texelSize", texelSize);

		screenMesh->Draw();
	}

	glDisable(GL_BLEND);

	/*=============================//
	//  결과 등록 (MainFBO 수정 X)  //
	//=============================*/
	context->SetTexture(RenderSlot::Bloom, m_bloomMips[0].texture.get());
	return true;
}

void KawaseBloomEffect::CreateBloomMips(int32 width, int32 height)
{
	m_bloomMips.clear();

	int32 mipWidth = width;
	int32 mipHeight = height;

	for (int32 i = 0; i < m_iteration; i++)
	{
		mipWidth >>= 1;
		mipHeight >>= 1;

		// 너무 작아지면 중단 (최소 2x2)
		if (mipWidth < 2 || mipHeight < 2) break;

		KawaseBloomMips mip;
		mip.width = mipWidth;
		mip.height = mipHeight;

		// R11G11B10F 같은 HDR 포맷 사용 권장
		mip.texture = Texture::Create(mipWidth, mipHeight, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);

		// Kawase 블러는 Linear 필수, Clamp 필수
		mip.texture->SetFilter(GL_LINEAR, GL_LINEAR);
		mip.texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		m_bloomMips.push_back(std::move(mip));
	}
}

void KawaseBloomEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
	CreateBloomMips(m_width, m_height);
}
