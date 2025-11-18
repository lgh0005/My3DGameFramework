#include "pch.h"
#include "EnvironmentRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/CubeTexture.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

EnvironmentRenderPassUPtr EnvironmentRenderPass::Create(ProgramUPtr program, CubeTexturePtr cubeTexture)
{
	auto pass = EnvironmentRenderPassUPtr(new EnvironmentRenderPass());
	if (!pass->Init(std::move(program), cubeTexture))
		return nullptr;
	return std::move(pass);
}

bool EnvironmentRenderPass::Init(ProgramUPtr program, CubeTexturePtr cubeTexture)
{
	if (!Super::Init(std::move(program)))
		return false;

	m_cubeTexture = cubeTexture;
	if (!m_cubeTexture) return false;

	return true;
}

void EnvironmentRenderPass::Render(Scene* scene, Camera* camera)
{
	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();
	auto cameraPos = camera->GetTransform().GetPosition();

	// TEMP : 이는 현재 큐브에 환경맵 텍스쳐를 입힌 것 뿐
	// 이후에 PBR 기반 렌더링 시 다르게 사용할 수 있음
	m_program->Use();

	m_program->SetUniform("view", view);
	m_program->SetUniform("projection", projection);
	m_program->SetUniform("cameraPos", cameraPos);

	m_cubeTexture->Bind();
	m_program->SetUniform("skybox", 0);

	for (const auto* renderer : m_renderers)
	{
		MeshPtr mesh = renderer->GetMesh();
		auto& transform = renderer->GetTransform();
		if (!mesh) continue;

		m_program->SetUniform("model", transform.GetModelMatrix());
		mesh->Draw(m_program.get());
	}
}