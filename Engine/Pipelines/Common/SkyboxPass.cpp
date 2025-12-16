#include "EnginePch.h"
#include "SkyboxPass.h"

#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Resources/CubeTexture.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Graphics/Program.h"
#include "Graphics/Geometry.h"
#include "Graphics/SkyLight.h"
#include "Components/Camera.h"

SkyboxPassUPtr SkyboxPass::Create()
{
	auto pass = SkyboxPassUPtr(new SkyboxPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool SkyboxPass::Init()
{
	m_skyboxProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Sky_Skybox.vert",
		"./Resources/Shaders/Common/Common_Sky_Skybox.frag"
	);
	if (!m_skyboxProgram) return false;

	m_cubeMesh = GeometryGenerator::CreateBox();
	if (!m_cubeMesh) return false;

	return true;
};

void SkyboxPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto camera = context->GetCamera();
	auto skyTexture = context->GetSkyLight()->GetSkybox();
	if (!skyTexture) return;

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_skyboxProgram->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_skyboxProgram->SetUniform("transform", transform);

	// 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	skyTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);

	m_cubeMesh->Draw(m_skyboxProgram.get());

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}