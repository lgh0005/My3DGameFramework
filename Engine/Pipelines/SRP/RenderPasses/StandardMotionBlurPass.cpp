#include "EnginePch.h"
#include "StandardMotionBlurPass.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/ScreenMesh.h"
#include "Resources/Texture.h"
#include "Framebuffers/PostProcessFramebuffer.h"

#include "Pipelines/SRP/StandardRenderContext.h"

DECLARE_DEFAULTS_IMPL(StandardMotionBlurPass)

StandardMotionBlurPassUPtr StandardMotionBlurPass::Create(int32 width, int32 height)
{
	auto pass = StandardMotionBlurPassUPtr(new StandardMotionBlurPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardMotionBlurPass::Init(int32 width, int32 height)
{
	m_motionBlurProgram = RESOURCE.GetResource<Program>("standard_motion_blur");
	m_plane = RESOURCE.GetResource<ScreenMesh>("Screen");
	m_blurFBO = PostProcessFramebuffer::Create(width, height, false);

	m_motionBlurProgram->Use();
	m_motionBlurProgram->SetUniform("uColorTexture", 0);    // 0번 슬롯: 원본 화면
	m_motionBlurProgram->SetUniform("uVelocityTexture", 1);

	return true;
}

void StandardMotionBlurPass::Render(RenderContext* context)
{
	auto stdCtx = (StandardRenderContext*)context;
	if (!stdCtx) return;

	// 1. Input 가져오기 (Context가 가리키는 현재 메인 화면)
	PostProcessFramebuffer* mainFBO = stdCtx->GetTargetFramebuffer();
	Texture* velocityTex = stdCtx->GetGBufferVelocity();

	// [Step 1] 블러 처리: Main FBO -> Blur FBO
	// 임시 버퍼에 그리기 준비
	m_blurFBO->Bind();
	glViewport(0, 0, m_blurFBO->GetWidth(), m_blurFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_motionBlurProgram->Use();

	// 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	mainFBO->GetColorAttachment(0)->Bind(); // 원본 읽기

	glActiveTexture(GL_TEXTURE1);
	velocityTex->Bind(); // 속도 읽기

	// 블러 강도 등 유니폼 설정 (필요시)
	m_motionBlurProgram->SetUniform("uBlurScale", 1.75f);

	// 그리기 (Quad)
	m_plane->Draw();

	// [Step 2] 결과 복사: Blur FBO -> Main FBO
	PostProcessFramebuffer::Blit
	(
		m_blurFBO.get(),    // Source (블러된 것)
		mainFBO,            // Dest   (파이프라인 원본)
		mainFBO->GetWidth(),
		mainFBO->GetHeight()
	);

	Framebuffer::BindToDefault();
}

