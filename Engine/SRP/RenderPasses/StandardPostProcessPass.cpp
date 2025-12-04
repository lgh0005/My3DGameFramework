#include "EnginePch.h"
#include "StandardPostProcessPass.h"

#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"    
#include "Components/Camera.h"

#include "SRP/StandardRenderContext.h"

StandardPostProcessPassUPtr StandardPostProcessPass::Create(int32 width, int32 height)
{
	auto pass = StandardPostProcessPassUPtr(new StandardPostProcessPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardPostProcessPass::Init(int32 width, int32 height)
{
	m_compositeProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Post_PostProcess.vert",
		"./Resources/Shaders/Standard/Post_PostProcess.frag"
	);
	m_blurProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Post_Blur.vert",
		"./Resources/Shaders/Standard/Post_Blur.frag"
	);
	if (!m_compositeProgram || !m_blurProgram) return false;

	m_plane = StaticMesh::CreateNDCQuad();
	if (!m_plane) return false;

	m_frameBuffer = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[0] = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[1] = Framebuffer::Create(width, height, 1);

	return (m_frameBuffer && m_pingPongFBOs[0] && m_pingPongFBOs[1]);
}

void StandardPostProcessPass::Render(RenderContext* context)
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

	glActiveTexture(GL_TEXTURE0);
	sceneTexture->Bind();
	m_compositeProgram->SetUniform("tex", 0);

	glActiveTexture(GL_TEXTURE1);
	finalBloomTexture->Bind();
	m_compositeProgram->SetUniform("bloomBlur", 1);

	m_plane->Draw(m_compositeProgram.get());
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
	/*glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}
