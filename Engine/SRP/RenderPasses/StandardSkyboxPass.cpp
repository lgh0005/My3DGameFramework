#include "EnginePch.h"
#include "StandardSkyboxPass.h"

#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Graphics/Program.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Geometry.h"
#include "Components/Camera.h"

#include "SRP/StandardRenderContext.h"

StandardSkyboxPassUPtr StandardSkyboxPass::Create()
{
	auto pass = StandardSkyboxPassUPtr(new StandardSkyboxPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool StandardSkyboxPass::Init()
{
	m_skyboxProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Sky_Skybox.vert",
		"./Resources/Shaders/Standard/Sky_Skybox.frag"
	);
	if (!m_skyboxProgram) return false;

	m_cubeMesh = GeometryGenerator::CreateBox();
	if (!m_cubeMesh) return false;

	return true;
};

void StandardSkyboxPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto stdCtx = (StandardRenderContext*)context;
	auto camera = context->GetCamera();
	auto skybox = nullptr;

	auto skyTexture = stdCtx->GetSkyboxTexture();
	if (!skyTexture) return;

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_skyboxProgram->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_skyboxProgram->SetUniform("transform", transform);

	skyTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);

	m_cubeMesh->Draw(m_skyboxProgram.get());

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}