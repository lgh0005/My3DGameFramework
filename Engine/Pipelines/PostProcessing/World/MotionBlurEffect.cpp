#include "EnginePch.h"
#include "MotionBlurEffect.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Graphics/Framebuffers/PostProcessFrameBuffer.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
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
	m_priority = priority;
	m_width = width;
	m_height = height;

	m_motionBlurProgram = RESOURCE.GetResource<GraphicsProgram>("common_motion_blur");
	if (!m_motionBlurProgram) return false;

	m_motionBlurProgram->Use();
	m_motionBlurProgram->SetUniform("uColorTexture", 0);
	m_motionBlurProgram->SetUniform("uVelocityTexture", 1);
	m_motionBlurProgram->SetUniform("uBlurScale", m_blurScale);
	return true;
}

bool MotionBlurEffect::Render(RenderContext* context, Framebuffer* srcFBO, Framebuffer* dstFBO, ScreenMesh* screenMesh)
{
	// 0. 필수 데이터 확인
	Texture* velocityTex = context->GetTexture(RenderSlot::GVelocity);
	if (!srcFBO || !dstFBO || !velocityTex) return false;

	// [Step 1] 내 작업 공간(Internal FBO) 준비
	dstFBO->Bind();
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 1번 슬롯: 원본 화면 (MainFBO에서 가져옴)
	m_motionBlurProgram->Use();
	glActiveTexture(GL_TEXTURE0);
	srcFBO->GetColorAttachment(0)->Bind();

	// 2번 슬롯: 속도 버퍼 (G-Buffer에서 가져옴)
	glActiveTexture(GL_TEXTURE1);
	velocityTex->Bind();

	// [Step 3] 그리기 (Internal FBO에 그려짐)
	// 깊이 테스트는 필요 없으니 끕니다.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	screenMesh->Draw();

	// 상태 복구 (다음 패스를 위해)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void MotionBlurEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
}
