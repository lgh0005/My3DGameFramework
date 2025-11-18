#include "EnginePch.h"
#include "PostProcessingPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"     
#include "Components/Camera.h"     

PostProcessingRenderPassUPtr PostProcessingRenderPass::Create(ProgramUPtr program, 
															  int32 width, int32 height,
															  MeshPtr planeMesh)
{
	auto pass = PostProcessingRenderPassUPtr(new PostProcessingRenderPass());
	if (!pass->Init(std::move(program), width, height, planeMesh)) return nullptr;
	return std::move(pass);
}

bool PostProcessingRenderPass::Init(ProgramUPtr program, int32 width, int32 height, MeshPtr planeMesh)
{
	if (!Super::Init(std::move(program))) return false;
	m_plane = planeMesh;
	m_frameBuffer = Framebuffer::Create(width, height, 4);
	return true;
}

void PostProcessingRenderPass::BeginDraw()
{
	m_frameBuffer->Bind();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void PostProcessingRenderPass::Render(Scene* scene, Camera* camera)
{
	m_frameBuffer->Resolve();
	Framebuffer::BindToDefault();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_program->Use();
	m_program->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
	m_program->SetUniform("gamma", m_gamma);
	m_frameBuffer->GetColorAttachment()->Bind();
	m_program->SetUniform("tex", 0);

	m_plane->Draw(m_program.get());
}
