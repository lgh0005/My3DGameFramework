#include "EnginePch.h"
#include "StandardPostProcessPass.h"

#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Graphics/Geometry.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/Texture.h"
#include "Components/Camera.h"
#include "Framebuffers/PostProcessFramebuffer.h"

#include "Pipelines/SRP/StandardRenderContext.h"

DECLARE_DEFAULTS_IMPL(StandardPostProcessPass)

StandardPostProcessPassUPtr StandardPostProcessPass::Create(int32 width, int32 height)
{
	auto pass = StandardPostProcessPassUPtr(new StandardPostProcessPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardPostProcessPass::Init(int32 width, int32 height)
{
	m_thresholdProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Standard_Post_Threshold.vert",
		"./Resources/Shaders/Standard/Standard_Post_Threshold.frag"
	);
	m_compositeProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Standard_Post_PostProcess.vert",
		"./Resources/Shaders/Standard/Standard_Post_PostProcess.frag"
	);
	m_blurProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Standard_Post_Blur.vert",
		"./Resources/Shaders/Standard/Standard_Post_Blur.frag"
	);
	if (!m_compositeProgram || !m_blurProgram || !m_thresholdProgram) return false;

	m_plane = ScreenMesh::Create();
	if (!m_plane) return false;

	m_frameBuffer	  = PostProcessFramebuffer::Create(width, height, true);
	m_pingPongFBOs[0] = PostProcessFramebuffer::Create(width, height, false);
	m_pingPongFBOs[1] = PostProcessFramebuffer::Create(width, height, false);

	return (m_frameBuffer && m_pingPongFBOs[0] && m_pingPongFBOs[1]);
}

void StandardPostProcessPass::Render(RenderContext* context)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 0. 메인 씬 텍스처 준비
	auto sceneTexture = m_frameBuffer->GetColorAttachment(0).get();

	// 1. 밝은 영역 추출
	ExtractBrightAreas(sceneTexture);

	// 2. 가우시안 블러 수행 (결과물 텍스처를 받아옴)
	Texture* finalBloomTexture = ComputeGaussianBlur();

	// 3. 최종 합성
	RenderCompositePass(sceneTexture, finalBloomTexture);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void StandardPostProcessPass::Resize(int32 width, int32 height)
{
	m_frameBuffer = PostProcessFramebuffer::Create(width, height, true);
	m_pingPongFBOs[0] = PostProcessFramebuffer::Create(width, height, false);
	m_pingPongFBOs[1] = PostProcessFramebuffer::Create(width, height, false);

	if (m_compositeProgram)
	{
		m_compositeProgram->Use();
		m_compositeProgram->SetUniform
		(
			"inverseScreenSize",
			glm::vec2(1.0f / (float)width, 1.0f / (float)height)
		);
	}
}

/*=============================================//
//   standard post-processing helper methods   //
//=============================================*/
void StandardPostProcessPass::ExtractBrightAreas(Texture* sceneTexture)
{
	// 밝은 영역을 추출해서 PingPong[0]에 그립니다.
	m_pingPongFBOs[0]->Bind();
	glViewport(0, 0, m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_thresholdProgram->Use();
	glActiveTexture(GL_TEXTURE0);
	sceneTexture->Bind();
	m_thresholdProgram->SetUniform("screenTexture", 0);

	m_plane->Draw();
}

Texture* StandardPostProcessPass::ComputeGaussianBlur()
{
	bool horizontal = true;
	bool firstDraw = true;
	int amount = 10; // 짝수 권장

	m_blurProgram->Use();
	for (uint32 i = 0; i < amount; i++)
	{
		// 이번에 그릴 대상 FBO 바인딩
		m_pingPongFBOs[horizontal]->Bind();
		m_blurProgram->SetUniform("horizontal", horizontal);

		// 이전 단계의 결과물(텍스처) 바인딩
		glActiveTexture(GL_TEXTURE0);
		if (firstDraw)
		{
			// 첫 번째는 Threshold 결과(PingPong[0])를 사용
			m_pingPongFBOs[0]->GetColorAttachment(0)->Bind();
			firstDraw = false;
		}
		else
		{
			// 그 외에는 반대쪽 FBO의 텍스처를 사용
			m_pingPongFBOs[!horizontal]->GetColorAttachment(0)->Bind();
		}

		m_blurProgram->SetUniform("image", 0);
		m_plane->Draw();

		// 방향 전환
		horizontal = !horizontal;
	}

	// 루프가 끝난 후, 최종 결과는 방금 그리기를 마친 FBO의 텍스처가 아니라
	// 마지막 루프에서 'Source'로 쓰려 했던 텍스처 쪽에 담겨 있습니다.
	// (loop가 짝수 번 돌면 horizontal은 다시 true가 된 상태로 종료됨 -> !horizontal인 false(0번)에 결과 있음)
	return m_pingPongFBOs[!horizontal]->GetColorAttachment(0).get();
}

void StandardPostProcessPass::RenderCompositePass(Texture* sceneTexture, Texture* bloomTexture)
{
	// 화면(Default Framebuffer)으로 복귀
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_compositeProgram->Use();
	m_compositeProgram->SetUniform("gamma", m_gamma);
	m_compositeProgram->SetUniform("exposure", m_exposure);
	m_compositeProgram->SetUniform("bloom", true);
	m_compositeProgram->SetUniform
	(
		"inverseScreenSize",
		glm::vec2
		(
			1.0f / (float)m_frameBuffer->GetWidth(),
			1.0f / (float)m_frameBuffer->GetHeight()
		)
	);

	// Slot 0: 원본 장면 (HDR)
	if (sceneTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		sceneTexture->Bind();
		m_compositeProgram->SetUniform("tex", 0);
	}

	// Slot 1: Bloom 텍스처
	if (bloomTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		bloomTexture->Bind();
		m_compositeProgram->SetUniform("bloomBlur", 1);
	}

	m_plane->Draw();
}
