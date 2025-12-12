#include "EnginePch.h"
#include "StandardPostProcessPass.h"

#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Graphics/Program.h"
#include "Graphics/ScreenMesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Geometry.h"
#include "Components/Camera.h"

#include "Pipelines/SRP/StandardRenderContext.h"

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

	m_frameBuffer = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[0] = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[1] = Framebuffer::Create(width, height, 1);

	return (m_frameBuffer && m_pingPongFBOs[0] && m_pingPongFBOs[1]);
}

void StandardPostProcessPass::Render(RenderContext* context)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 0. 메인 씬 텍스처 준비
	m_frameBuffer->Resolve();
	auto sceneTexture = m_frameBuffer->GetColorAttachment(0);

	// [STEP 1] 밝은 영역 추출 (Threshold Pass)
	m_pingPongFBOs[0]->Bind();
	glViewport(0, 0, m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());

	// 배경을 검은색으로 밀어야 추출되지 않은 부분이 어둡게 나옵니다.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_thresholdProgram->Use();
	glActiveTexture(GL_TEXTURE0);
	sceneTexture->Bind(); // 원본(HDR)을 넣습니다.
	m_thresholdProgram->SetUniform("screenTexture", 0);

	// 여기서 Skybox(밝기 1.5)는 1.0을 넘으므로 살아남아 FBO[0]에 기록됩니다.
	m_plane->Draw();

	// [STEP 2] 가우시안 블러 (Ping-Pong Blur)
	bool horizontal = true;
	bool firstDraw = true;
	int amount = 10; // 블러 반복 횟수 (짝수로 맞추는 게 좋음)

	m_blurProgram->Use();
	for (uint32 i = 0; i < amount; i++)
	{
		m_pingPongFBOs[horizontal]->Bind();
		m_blurProgram->SetUniform("horizontal", horizontal);

		glActiveTexture(GL_TEXTURE0);
		if (firstDraw)
		{
			m_pingPongFBOs[0]->GetColorAttachment(0)->Bind();
			firstDraw = false;
		}
		else
		{
			m_pingPongFBOs[!horizontal]->GetColorAttachment(0)->Bind();
		}

		m_blurProgram->SetUniform("image", 0);
		m_plane->Draw(m_blurProgram.get());

		// 다음 루프를 위해 방향 전환 (Ping <-> Pong)
		horizontal = !horizontal;
	}

	// [STEP 3] 최종 합성 (Composite & Tone Mapping)
	Framebuffer::BindToDefault();
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

	// 텍스처 슬롯 0 : 원본 장면 (HDR)
	glActiveTexture(GL_TEXTURE0);
	sceneTexture->Bind();
	m_compositeProgram->SetUniform("tex", 0);

	// 텍스처 슬롯 1 : 최종 블러 처리된 Bloom 텍스처
	// 루프가 끝난 후 !horizontal 쪽에 최종 결과가 있습니다.
	auto finalBloomTexture = m_pingPongFBOs[!horizontal]->GetColorAttachment(0);
	glActiveTexture(GL_TEXTURE1);
	finalBloomTexture->Bind();
	m_compositeProgram->SetUniform("bloomBlur", 1);
	m_plane->Draw();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void StandardPostProcessPass::Resize(int32 width, int32 height)
{
	m_frameBuffer = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[0] = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[1] = Framebuffer::Create(width, height, 1);

	if (m_compositeProgram)
	{
		m_compositeProgram->Use();
		m_compositeProgram->SetUniform("inverseScreenSize",
			glm::vec2(1.0f / (float)width, 1.0f / (float)height));
	}
}

void StandardPostProcessPass::BeginDraw()
{
	m_frameBuffer->Bind();
	glViewport(0, 0, m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}
