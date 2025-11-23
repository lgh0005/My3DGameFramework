#include "pch.h"
#include "GeometryRenderPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/FrameBuffer.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"

GeometryRenderPassUPtr GeometryRenderPass::Create
(
	ProgramUPtr staticGeometryProgram,
	ProgramUPtr skinnedGeometryProgram,
	int32 width, int32 height
)
{
	auto pass = GeometryRenderPassUPtr(new GeometryRenderPass());
	if (!pass->Init(
		std::move(staticGeometryProgram), 
		std::move(skinnedGeometryProgram),
		width, height))
		return nullptr;
	return std::move(pass);
}

bool GeometryRenderPass::Init
(
	ProgramUPtr staticGeometryProgram,
	ProgramUPtr skinnedGeometryProgram,
	int32 width, int32 height
)
{
	m_staticGeometryProgram = std::move(staticGeometryProgram);
	m_skinnedGeometryProgram = std::move(skinnedGeometryProgram);
	m_gBuffer = Framebuffer::CreateGBuffer(width, height);
	if (!m_gBuffer) return false;
	return true;
}

void GeometryRenderPass::Render(Scene* scene, Camera* camera)
{
	// 1. G-Buffer FBO 바인딩
	m_gBuffer->Bind();

	// 2. 화면 클리어
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// 3. 공통 행렬 및 카메라 데이터 준비
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 projection = camera->GetProjectionMatrix();
	glm::vec3 cameraPos = camera->GetTransform().GetPosition();

	// 4. Static Mesh 그리기 (정적 오브젝트)
	if (m_staticGeometryProgram) 
	{
		m_staticGeometryProgram->Use();
		m_staticGeometryProgram->SetUniform("view", view);
		m_staticGeometryProgram->SetUniform("projection", projection);
		m_staticGeometryProgram->SetUniform("viewPos", cameraPos);

		auto staticPass = scene->GetRenderPass("Static");
		if (staticPass)
		{
			for (const auto* renderer : staticPass->GetRenderers())
			{
				MeshPtr mesh = renderer->GetMesh();
				auto model = renderer->GetTransform().GetModelMatrix();
				auto material = renderer->GetMaterial();

				material->SetToProgram(m_staticGeometryProgram.get());
				m_staticGeometryProgram->SetUniform("model", model);

				mesh->Draw(m_staticGeometryProgram.get());
			}
		}
	}

	// 5. Skinned Mesh 그리기 (애니메이션 오브젝트)
	if (m_skinnedGeometryProgram)
	{
		m_skinnedGeometryProgram->Use();
		m_skinnedGeometryProgram->SetUniform("view", view);
		m_skinnedGeometryProgram->SetUniform("projection", projection);
		m_skinnedGeometryProgram->SetUniform("viewPos", cameraPos);

		auto skinnedPass = scene->GetRenderPass("Skinned");
		if (skinnedPass)
		{
			for (const auto* renderer : skinnedPass->GetRenderers())
			{
				GameObject* go = renderer->GetOwner();
				MeshPtr mesh = renderer->GetMesh();
				auto material = renderer->GetMaterial();
				auto& transform = go->GetTransform();
				Animator* animator = go->GetComponent<Animator>();

				material->SetToProgram(m_skinnedGeometryProgram.get());

				auto finalMatrices = animator->GetFinalBoneMatrices();
				for (int i = 0; i < finalMatrices.size(); ++i)
					m_skinnedGeometryProgram->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);

				m_skinnedGeometryProgram->SetUniform("model", transform.GetModelMatrix());

				mesh->Draw(m_skinnedGeometryProgram.get());
			}
		}
	}

	// 6. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}