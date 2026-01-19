#include "EnginePch.h"
#include "MotionBlurPass.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"

DECLARE_DEFAULTS_IMPL(MotionBlurPass)

MotionBlurPassUPtr MotionBlurPass::Create(int32 width, int32 height)
{
	auto pass = MotionBlurPassUPtr(new MotionBlurPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool MotionBlurPass::Init(int32 width, int32 height)
{
	m_motionBlurProgram = RESOURCE.GetResource<Program>("common_motion_blur");
	m_plane = RESOURCE.GetResource<ScreenMesh>("Screen");
	m_blurFBO = PostProcessFramebuffer::Create(width, height);
	if (!m_motionBlurProgram || !m_plane || !m_blurFBO) return false;

	m_motionBlurProgram->Use();
	m_motionBlurProgram->SetUniform("uColorTexture", 0);    // 0번 슬롯: 원본 화면
	m_motionBlurProgram->SetUniform("uVelocityTexture", 1);

	return true;
}

void MotionBlurPass::Render(RenderContext* context)
{
	// 1. Input 가져오기 (Context가 가리키는 현재 메인 화면)
	Framebuffer* mainFBO = context->GetTargetFramebuffer();
	Texture* velocityTex = context->GetTexture(RenderSlot::GVelocity);
	if (!mainFBO || !velocityTex) return;

	// [Step 1] 블러 처리: Main FBO -> Blur FBO
	// 임시 버퍼에 그리기 준비
	m_blurFBO->Bind();
	glViewport(0, 0, m_blurFBO->GetWidth(), m_blurFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_motionBlurProgram->Use();

	// 원본 화면(mainFBO의 0번 텍스처) 바인딩
	glActiveTexture(GL_TEXTURE0);
	mainFBO->GetColorAttachment(0)->Bind();

	// G-Buffer의 속도 정보 바인딩
	glActiveTexture(GL_TEXTURE1);
	velocityTex->Bind();

	// 블러 강도 등 유니폼 설정 (필요시)
	m_motionBlurProgram->SetUniform("uBlurScale", 0.95f); // 0.95f 정도

	// 그리기 (Quad)
	m_plane->Draw();

	// [Step 2] 결과 복사: Blur FBO -> Main FBO
	Framebuffer::Blit(m_blurFBO.get(), mainFBO, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// 다음 패스를 위해 상태 복구 및 바인딩 해제
	glEnable(GL_DEPTH_TEST);
	Framebuffer::BindToDefault();
}

void MotionBlurPass::Resize(int32 width, int32 height)
{
	m_blurFBO->OnResize(width, height);
}