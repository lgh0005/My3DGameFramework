#include "EnginePch.h"
#include "StandardSkyboxPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Components/Camera.h"

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
		"./Resources/Shaders/Skybox/skybox.vert",
		"./Resources/Shaders/Skybox/skybox.frag"
	);
	if (!m_skyboxProgram) return false;

	m_cubeMesh = StaticMesh::CreateBox();
	if (!m_cubeMesh) return false;

	return true;
};

// TODO : Render 추상 메서드 생김새를 조금 다듬을 필요는 있음
void StandardSkyboxPass::Render(Scene* scene, Camera* camera)
{
	auto skyTexture = scene->GetSkyboxTexture();
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