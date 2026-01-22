#include "EnginePch.h"
#include "GaussianBloomEffect.h"
#include "Resources/Program.h"
#include "Graphics/Framebuffers/PostProcessFrameBuffer.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Meshes/ScreenMesh.h"

DECLARE_DEFAULTS_IMPL(GaussianBloomEffect)

GaussianBloomEffectUPtr GaussianBloomEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effect = GaussianBloomEffectUPtr(new GaussianBloomEffect());
	if (!effect->Init(priority, width, height)) return nullptr;
	return std::move(effect);
}

bool GaussianBloomEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	m_thresholdProgram = RESOURCE.GetResource<Program>("standard_postprocess_threshold");
	m_blurProgram = RESOURCE.GetResource<Program>("standard_postprocess_blur");
	m_pingPongFBOs[0] = PostProcessFramebuffer::Create(width, height);
	m_pingPongFBOs[1] = PostProcessFramebuffer::Create(width, height);

	m_thresholdProgram->Use();
	m_thresholdProgram->SetUniform("screenTexture", 0);

	return (m_thresholdProgram && m_blurProgram && m_pingPongFBOs[0] && m_pingPongFBOs[1]);
}

bool GaussianBloomEffect::Render(RenderContext* context, Framebuffer* mainFBO, ScreenMesh* screenMesh)
{
	if (!context || !mainFBO) return false;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 1. 밝은 영역 추출 (MainFBO -> PingPong[0])
	// MainFBO의 0번 텍스처(현재 화면)를 입력으로 사용
	ExtractBrightAreas(mainFBO->GetColorAttachment(0).get(), screenMesh);

	// 2. 가우시안 블러 수행 (PingPong[0] <-> PingPong[1])
	Texture* finalBloom = ComputeGaussianBlur(screenMesh);

	// 3. 결과를 MainFBO에 그리는 게 아니라, Context에 등록함.
	// 나중에 DisplayMappingEffect가 이 텍스처를 가져다가 합칠 것임.
	context->SetTexture(RenderSlot::Bloom, finalBloom);

	return true;
}

void GaussianBloomEffect::ExtractBrightAreas(Texture* src, ScreenMesh* mesh)
{
	m_pingPongFBOs[0]->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT); // Clear는 필수

	m_thresholdProgram->Use();
	glActiveTexture(GL_TEXTURE0);
	src->Bind();

	mesh->Draw();
}

Texture* GaussianBloomEffect::ComputeGaussianBlur(ScreenMesh* mesh)
{
	bool horizontal = true;
	bool firstDraw = true;
	int amount = 10; // 블러 반복 횟수

	m_blurProgram->Use();
	for (unsigned int i = 0; i < amount; i++)
	{
		m_pingPongFBOs[horizontal]->Bind();
		m_blurProgram->SetUniform("horizontal", horizontal);

		glActiveTexture(GL_TEXTURE0);
		if (firstDraw)
		{
			// 첫 단계에선 Threshold 결과물 사용
			m_pingPongFBOs[0]->GetColorAttachment(0)->Bind();
			firstDraw = false;
		}
		else
		{
			// 그 다음부턴 방금 그린 반대쪽 FBO 사용
			m_pingPongFBOs[!horizontal]->GetColorAttachment(0)->Bind();
		}

		m_blurProgram->SetUniform("image", 0);
		mesh->Draw();

		horizontal = !horizontal;
	}

	// 마지막 루프가 끝난 뒤, 결과가 담긴 쪽(!horizontal)의 텍스처 반환
	return m_pingPongFBOs[!horizontal]->GetColorAttachment(0).get();
}

void GaussianBloomEffect::OnResize(int32 width, int32 height)
{
	Super::OnResize(width, height);
	if (m_pingPongFBOs[0]) m_pingPongFBOs[0]->OnResize(m_width, m_height);
	if (m_pingPongFBOs[1]) m_pingPongFBOs[1]->OnResize(m_width, m_height);
}