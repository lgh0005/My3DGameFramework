#include "EnginePch.h"
#include "PostProcessPass.h"
#include "Graphics/Framebuffers/Framebuffer.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"
#include "Pipelines/PostProcessing/PostProcessEffect.h"
#include "Resources/Meshes/ScreenMesh.h"

DECLARE_DEFAULTS_IMPL(PostProcessPass)

PostProcessPassUPtr PostProcessPass::Create(int32 width, int32 height)
{
	auto pass = PostProcessPassUPtr(new PostProcessPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool PostProcessPass::Init(int32 width, int32 height)
{
	m_screen = ScreenMesh::Create();
	m_frameBuffer = PostProcessFramebuffer::Create(width, height);
	if (!m_screen || !m_frameBuffer) return false;
	
	Resize(width, height);
	return true;
}

void PostProcessPass::Render(RenderContext* context)
{
	// 0. 메인 FBO가 유효한지 확인
	if (!m_frameBuffer) return;

	// 1. 이펙트 순회
	for (const auto& effect : m_effects)
	{
		if (effect->IsEffectEnable())
		{
			effect->Render
			(
				context, 
				m_frameBuffer.get(),
				m_screen.get()
			);
		}
	}

	// 2. 모든 처리가 끝난 후 화면에 출력
	PresentToScreen();
}

void PostProcessPass::PresentToScreen()
{
	if (!m_frameBuffer) return;

	// 1. 기본 백버퍼(화면) 바인딩 및 클리어
	Framebuffer::BindToDefault();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. Main FBO의 내용을 화면으로 고속 복사 (Blit)
	int32 srcW = m_frameBuffer->GetWidth();
	int32 srcH = m_frameBuffer->GetHeight();
	int32 dstW = WINDOW.GetWindowWidth();
	int32 dstH = WINDOW.GetWindowHeight();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer
	(
		0, 0, srcW, srcH,
		0, 0, dstW, dstH,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	Framebuffer::BindToDefault();
}

void PostProcessPass::Resize(int32 width, int32 height)
{
	// 1. 메인 프레임버퍼 리사이즈
	if (m_frameBuffer)
		m_frameBuffer->OnResize(width, height);

	// 2. 등록된 모든 이펙트에게 리사이즈 전파
	for (const auto& effect : m_effects)
		effect->OnResize(width, height);
}

void PostProcessPass::AddEffect(PostProcessEffectUPtr effect)
{
	if (!effect) return;
	m_effects.push_back(std::move(effect));
}

