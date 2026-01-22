#include "EnginePch.h"
#include "MotionBlurEffect.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Framebuffers/PostProcessFrameBuffer.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(MotionBlurEffect)

MotionBlurEffectUPtr MotionBlurEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effect = MotionBlurEffectUPtr(new MotionBlurEffect());
	if (!effect->Init(priority, width, height)) return nullptr;
	return std::move(effect);
}

bool MotionBlurEffect::Init(int32 priority, int32 width, int32 height)
{
	m_motionBlurProgram = RESOURCE.GetResource<Program>("common_motion_blur");
	m_motionBlurFBO = PostProcessFramebuffer::Create(width, height);
	if (!m_motionBlurProgram || !m_motionBlurFBO) return false;
	return true;
}

bool MotionBlurEffect::Render(RenderContext* context, Framebuffer* mainFBO, ScreenMesh* screenMesh)
{
	// 0. 필수 데이터 확인
	Texture* velocityTex = context->GetTexture(RenderSlot::GVelocity);
	if (!mainFBO || !velocityTex) return false;

	// [Step 1] 내 작업 공간(Internal FBO) 준비
	m_motionBlurFBO->Bind();
	glViewport(0, 0, m_motionBlurFBO->GetWidth(), m_motionBlurFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 1번 슬롯: 원본 화면 (MainFBO에서 가져옴)
	m_motionBlurProgram->Use();
	glActiveTexture(GL_TEXTURE0);
	mainFBO->GetColorAttachment(0)->Bind();
	m_motionBlurProgram->SetUniform("uColorTexture", 0);

	// 2번 슬롯: 속도 버퍼 (G-Buffer에서 가져옴)
	glActiveTexture(GL_TEXTURE1);
	velocityTex->Bind();
	m_motionBlurProgram->SetUniform("uVelocityTexture", 1);

	float blurScale = 1.0f;
	m_motionBlurProgram->SetUniform("uBlurScale", blurScale);

	// [Step 3] 그리기 (Internal FBO에 그려짐)
	// 깊이 테스트는 필요 없으니 끕니다.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	screenMesh->Draw();

	// [Step 4] 결과 반영 (Internal FBO -> Main FBO)
	// 내가 그린 블러된 이미지를 다시 메인 도화지에 '고속 복사' 합니다.
	// TODO : 이후에 복사 비용 최적화를 위해서
	// 1) Texture Swap, Framebuffer Swap 방식을 이용할 필요가 있음.
	Framebuffer::Blit
	(
		m_motionBlurFBO.get(), // Source: 내 작업물
		mainFBO,               // Dest: 메인 도화지
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);

	// [Step 5] 상태 복구 (다음 패스를 위해)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void MotionBlurEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
	m_motionBlurFBO->OnResize(width, height);
}
