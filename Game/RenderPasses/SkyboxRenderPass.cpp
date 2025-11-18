#include "pch.h"
#include "SkyboxRenderPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"

SkyboxRenderPassUPtr SkyboxRenderPass::Create(ProgramUPtr program, MeshPtr boxMesh, CubeTexturePtr cubeTexture)
{
    auto pass = SkyboxRenderPassUPtr(new SkyboxRenderPass());
    if (!pass->Init(std::move(program), boxMesh, cubeTexture))
		return nullptr;
    return std::move(pass);
}

bool SkyboxRenderPass::Init(ProgramUPtr program, MeshPtr boxMesh, CubeTexturePtr cubeTexture)
{
	if (!Super::Init(std::move(program)))
		return false;

	m_cubeMesh = boxMesh;
	m_cubeTexture = cubeTexture;
	return (m_cubeMesh && m_cubeTexture);
}

void SkyboxRenderPass::Render(Scene* scene, Camera* camera)
{
	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_program->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_program->SetUniform("transform", transform);

	m_cubeTexture->Bind();
	m_program->SetUniform("skybox", 0);

	m_cubeMesh->Draw(m_program.get());

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}
