#include "pch.h"
#include "EnvironmentRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/CubeTexture.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

EnvironmentRenderPassUPtr EnvironmentRenderPass::Create(ProgramUPtr program, CubeTexturePtr cubeTexture)
{
	auto pass = EnvironmentRenderPassUPtr(new EnvironmentRenderPass());
	if (!pass->Init(std::move(program), cubeTexture)) return nullptr;
	return std::move(pass);
}

bool EnvironmentRenderPass::Init(ProgramUPtr program, CubeTexturePtr cubeTexture)
{
	m_envProgram = std::move(program);
	if (!m_envProgram) return false;

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
	m_envProgram->Use();

	// TOOD : 이제 이것 역시도 UBO로 대체 할 필요 있음
	m_envProgram->SetUniform("view", view);
	m_envProgram->SetUniform("projection", projection);
	m_envProgram->SetUniform("cameraPos", cameraPos);

	m_cubeTexture->Bind();
	m_envProgram->SetUniform("skybox", 0);

	for (const auto* renderer : m_renderers)
	{
		MeshPtr mesh = renderer->GetMesh();
		auto& transform = renderer->GetTransform();
		if (!mesh) continue;

		m_envProgram->SetUniform("model", transform.GetWorldMatrix());
		mesh->Draw(m_envProgram.get());
	}
}

const std::vector<StaticMeshRenderer*>& EnvironmentRenderPass::GetRenderers() const
{
	return m_renderers;
}

void EnvironmentRenderPass::AddRenderer(StaticMeshRenderer* meshRenderer)
{
	m_renderers.push_back(meshRenderer);
}
