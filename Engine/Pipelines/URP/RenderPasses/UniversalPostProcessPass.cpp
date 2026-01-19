#include "EnginePch.h"
#include "UniversalPostProcessPass.h"

#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/Texture.h"
#include "Graphics/Geometry.h"
#include "Components/Camera.h"
#include "Framebuffers/PostProcessFrameBuffer.h"
#include "Framebuffers/EffectFramebuffer.h"

DECLARE_DEFAULTS_IMPL(UniversalPostProcessPass)

UniversalPostProcessPassUPtr UniversalPostProcessPass::Create(int32 width, int32 height)
{
	auto pass = UniversalPostProcessPassUPtr(new UniversalPostProcessPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool UniversalPostProcessPass::Init(int32 width, int32 height)
{
	m_bloomProgram = RESOURCE.GetResource<Program>("universal_postprocess_blur");
	m_compositeProgram = RESOURCE.GetResource<Program>("universal_postprocess_postprocess");
	m_plane = RESOURCE.GetResource<ScreenMesh>("Screen");

	// 1. 프레임 버퍼 생성
	m_frameBuffer = PostProcessFramebuffer::Create(width, height);
	if (!m_frameBuffer) return false;

	// 2. Bloom용 FBO 및 텍스처 생성 (추가됨)
	m_bloomFBO = EffectFramebuffer::CreateEmpty();
	if (!m_bloomFBO) return false;

	// 3. Bloom 밉맵 체인 생성
	CreateBloomMips(width, height);

	return true;
}

void UniversalPostProcessPass::Render(RenderContext* context)
{
	// 0. Lighting Pass에서 그려진 원본 HDR 이미지 가져오기
	auto hdrTexture = m_frameBuffer->GetColorAttachment(0).get();
	if (!hdrTexture || m_bloomMips.empty()) return;

	// 1. Kawase Bloom Pass (Extract -> Downsample -> Upsample)
	Texture* bloomTexture = RenderKawaseBloom(hdrTexture);

	// 2. Final Composite (Scene + BloomResult + ToneMapping + FXAA)
	RenderComposite(hdrTexture, bloomTexture);
}

void UniversalPostProcessPass::Resize(int32 width, int32 height)
{
	m_frameBuffer->OnResize(width, height);

	// Bloom Mip Chain 재생성 (Init 코드와 동일한 로직)
	CreateBloomMips(width, height);
}

/*==============================================//
//   universal post-processing helper methods   //
//==============================================*/
Texture* UniversalPostProcessPass::RenderKawaseBloom(Texture* hdrTexture)
{
	if (m_bloomMips.empty()) return nullptr;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_bloomFBO->Bind();
	m_bloomProgram->Use();

	/*====================================================//
	//  Prefilter & First Downsample (Source -> Mip[0])   //
	//====================================================*/
	m_bloomProgram->SetUniform("threshold", m_threshold);

	// [수정] AttachTexture에 0번 인덱스 명시
	m_bloomFBO->AttachColorTexture(0, m_bloomMips[0].texture);
	glViewport(0, 0, m_bloomMips[0].width, m_bloomMips[0].height);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	hdrTexture->Bind();
	m_bloomProgram->SetUniform("mainTexture", 0);

	glm::vec2 texelSize = { 1.0f / hdrTexture->GetWidth(), 1.0f / hdrTexture->GetHeight() };
	m_bloomProgram->SetUniform("texelSize", texelSize);
	m_bloomProgram->SetUniform("mode", 0); // Prefilter

	m_plane->Draw();

	/*==========================================//
	//  Downsample Chain (Mip[i-1] -> Mip[i])   //
	//==========================================*/
	m_bloomProgram->SetUniform("mode", 1);

	for (usize i = 1; i < m_bloomMips.size(); i++)
	{
		// Target: Current Mip
		m_bloomFBO->AttachColorTexture(0, m_bloomMips[i].texture);
		glViewport(0, 0, m_bloomMips[i].width, m_bloomMips[i].height);
		glClear(GL_COLOR_BUFFER_BIT);

		// Source: Previous Mip
		glActiveTexture(GL_TEXTURE0);
		m_bloomMips[i - 1].texture->Bind();
		m_bloomProgram->SetUniform("mainTexture", 0);

		texelSize = { 1.0f / m_bloomMips[i - 1].width, 1.0f / m_bloomMips[i - 1].height };
		m_bloomProgram->SetUniform("texelSize", texelSize);

		m_plane->Draw();
	}

	/*========================================//
	//  Upsample Chain (Mip[i] -> Mip[i-1])   //
	//========================================*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	m_bloomProgram->SetUniform("mode", 2);

	for (usize i = m_bloomMips.size() - 1; i > 0; i--)
	{
		// Target: Upper Mip (이미 그려진 것 위에 덧칠)
		m_bloomFBO->AttachColorTexture(0, m_bloomMips[i - 1].texture);
		glViewport(0, 0, m_bloomMips[i - 1].width, m_bloomMips[i - 1].height);

		// Source: Current Mip
		glActiveTexture(GL_TEXTURE0);
		m_bloomMips[i].texture->Bind();
		m_bloomProgram->SetUniform("mainTexture", 0);

		texelSize = { 1.0f / m_bloomMips[i].width, 1.0f / m_bloomMips[i].height };
		m_bloomProgram->SetUniform("texelSize", texelSize);

		m_plane->Draw();
	}

	glDisable(GL_BLEND);

	// 최종 결과는 가장 큰 밉맵(Mip[0])에 누적
	return m_bloomMips[0].texture.get();
}

void UniversalPostProcessPass::RenderComposite(Texture* hdrTexture, Texture* bloomTexture)
{
	if (!hdrTexture) return;

	// 최종 화면(Backbuffer)에 그릴 준비
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, hdrTexture->GetWidth(), hdrTexture->GetHeight());

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_compositeProgram->Use();
	m_compositeProgram->SetUniform("gamma", m_gamma);
	m_compositeProgram->SetUniform("exposure", m_exposure);

	// 텍스처 0번 슬롯 바인딩 (HDR 원본)
	glActiveTexture(GL_TEXTURE0);
	hdrTexture->Bind();
	m_compositeProgram->SetUniform("screenTexture", 0);
	if (bloomTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		bloomTexture->Bind();
		m_compositeProgram->SetUniform("bloomTexture", 1);
		m_compositeProgram->SetUniform("bloomStrength", 0.04f);
	}
	else
	{
		m_compositeProgram->SetUniform("bloomStrength", 0.0f);
	}

	// FXAA
	glm::vec2 invSize = { 1.0f / (float)hdrTexture->GetWidth(), 1.0f / (float)hdrTexture->GetHeight() };
	m_compositeProgram->SetUniform("inverseScreenSize", invSize);
	m_compositeProgram->SetUniform("useFXAA", true);

	m_plane->Draw();

	// 상태 복구
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void UniversalPostProcessPass::CreateBloomMips(int32 width, int32 height)
{
	m_bloomMips.clear();

	int32 mipWidth = width;
	int32 mipHeight = height;
	for (int32 i = 0; i < 5; i++) // 5단계 정도의 Mip Chain 생성
	{
		mipWidth >>= 1;
		mipHeight >>= 1;
		if (width < 2 || height < 2) break;

		KawaseBloomMips mip;
		mip.width = mipWidth;
		mip.height = mipHeight;
		mip.texture = Texture::Create(mipWidth, mipHeight, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);

		// 중요: Linear 필터링과 Clamp 설정
		mip.texture->SetFilter(GL_LINEAR, GL_LINEAR);
		mip.texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		m_bloomMips.push_back(std::move(mip));
	}
}

