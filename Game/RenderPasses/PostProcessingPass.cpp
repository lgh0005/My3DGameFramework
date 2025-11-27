#include "EnginePch.h"
#include "PostProcessingPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"    
#include "Components/Camera.h"

PostProcessingRenderPassUPtr PostProcessingRenderPass::Create
(
	ProgramUPtr finalCompositeProgram, ProgramUPtr blurProgram,
	int32 width, int32 height, MeshPtr planeMesh
)
{
	auto pass = PostProcessingRenderPassUPtr(new PostProcessingRenderPass());
	if (!pass->Init(
		std::move(finalCompositeProgram), std::move(blurProgram),
		width, height, planeMesh)) return nullptr;
	return std::move(pass);
}

bool PostProcessingRenderPass::Init
(
	ProgramUPtr finalCompositeProgram, ProgramUPtr blurProgram,
	int32 width, int32 height, MeshPtr planeMesh
)
{
	if (!Super::Init(std::move(finalCompositeProgram))) return false;
	m_blurProgram = std::move(blurProgram);
	m_plane = planeMesh;

	m_frameBuffer = Framebuffer::Create(width, height, 1);
	if (!m_frameBuffer) return false;

	m_pingPongFBOs[0] = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[1] = Framebuffer::Create(width, height, 1);

	if (!m_pingPongFBOs[0] || !m_pingPongFBOs[1])
		return false;

	return true;
}

void PostProcessingRenderPass::Render(Scene* scene, Camera* camera)
{
	m_frameBuffer->Resolve();

	auto sceneTexture = m_frameBuffer->GetColorAttachment(0);
	auto brightTexture = m_frameBuffer->GetColorAttachment(1);

	bool horizontal = true;
	bool firstDraw = true;
	int amount = 10;

	m_blurProgram->Use();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());
	for (uint32 i = 0; i < amount; i++)
	{
		m_pingPongFBOs[horizontal]->Bind();
		m_blurProgram->SetUniform("horizontal", horizontal);

		glActiveTexture(GL_TEXTURE0);
		if (firstDraw)
		{
			brightTexture->Bind();
			firstDraw = false;
		}
		else
		{
			m_pingPongFBOs[!horizontal]->GetColorAttachment(0)->Bind();
		}

		m_blurProgram->SetUniform("image", 0);
		m_plane->Draw(m_blurProgram.get());
		m_pingPongFBOs[horizontal]->Resolve();
		horizontal = !horizontal;
	}

	auto finalBloomTexture = m_pingPongFBOs[!horizontal]->GetColorAttachment(0);

	Framebuffer::BindToDefault();
	glViewport(0, 0, m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program->Use();
	m_program->SetUniform("gamma", m_gamma);
	m_program->SetUniform("exposure", m_exposure);
	m_program->SetUniform("bloom", true);
	m_program->SetUniform
	(
		"inverseScreenSize",
		glm::vec2
		(
			1.0f / (float)m_frameBuffer->GetWidth(),
			1.0f / (float)m_frameBuffer->GetHeight()
		)
	);

	glActiveTexture(GL_TEXTURE0);
	sceneTexture->Bind();
	m_program->SetUniform("tex", 0);

	glActiveTexture(GL_TEXTURE1);
	finalBloomTexture->Bind();
	m_program->SetUniform("bloomBlur", 1);

	m_plane->Draw(m_program.get());
}
