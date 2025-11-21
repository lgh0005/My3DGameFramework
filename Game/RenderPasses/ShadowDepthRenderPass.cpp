#include "EnginePch.h"
#include "ShadowDepthRenderPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"     
#include "Graphics/ShadowMap.h"
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"

ShadowDepthRenderPassUPtr ShadowDepthRenderPass::Create
(
	ProgramUPtr staticDepthProgram,
	ProgramUPtr skinnedDepthProgram,
	int32 resolution
)
{
	auto pass = ShadowDepthRenderPassUPtr(new ShadowDepthRenderPass());
	if (!pass->Init(std::move(staticDepthProgram), std::move(skinnedDepthProgram), 
		resolution)) return nullptr;
	return std::move(pass);
}

bool ShadowDepthRenderPass::Init
(
	ProgramUPtr staticDepthProgram,
	ProgramUPtr skinnedDepthProgram,
	int32 resolution
)
{
	m_resolution = resolution;
	m_staticDepthProgram  = std::move(staticDepthProgram);
	m_skinnedDepthProgram = std::move(skinnedDepthProgram);
	m_shadowMap = ShadowMap::Create(resolution, resolution);
	if (!m_shadowMap) return false;
	return true;
}

void ShadowDepthRenderPass::Render(Scene* scene, Camera* camera)
{
	/* 여기서 각자의 깊이를 계산해야 함. */
	// 1. 조명 입장에서의 좌표계로 전환하는 행렬 계산
	CalculateLightSpaceMatrix(scene);

	// 4. 렌더링 준비
	m_shadowMap->Bind();
	auto shadowMap = m_shadowMap->GetShadowMap();
	glViewport(0, 0, shadowMap->GetWidth(), shadowMap->GetHeight());
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	if (m_staticDepthProgram)
	{
		m_staticDepthProgram->Use();
		m_staticDepthProgram->SetUniform("lightSpaceMatrix", m_lightSpaceMatrix);

		// Scene에서 랜더 패스 'Static'에서 렌더러들을 가져옴
		const auto& staticRenderers = scene->GetRenderPass("Static")->GetRenderers();
		for (const auto* renderer : staticRenderers)
		{
			auto model = renderer->GetTransform().GetModelMatrix();
			m_staticDepthProgram->SetUniform("model", model);
			renderer->GetMesh()->Draw(m_staticDepthProgram.get());
		}
	}

	if (m_skinnedDepthProgram)
	{
		m_skinnedDepthProgram->Use();
		m_skinnedDepthProgram->SetUniform("lightSpaceMatrix", m_lightSpaceMatrix);

		// Scene에서 랜더 패스 'Skinned'에서 렌더러들을 가져옴
		const auto& skinnedRenderers = scene->GetRenderPass("Skinned")->GetRenderers();
		for (const auto* renderer : skinnedRenderers)
		{
			GameObject* go = renderer->GetOwner();
			auto model = renderer->GetTransform().GetModelMatrix();
			m_skinnedDepthProgram->SetUniform("model", model);

			Animator* animator = go->GetComponent<Animator>();
			if (animator)
			{
				auto finalMatrices = animator->GetFinalBoneMatrices();
				for (int i = 0; i < finalMatrices.size(); ++i)
					m_skinnedDepthProgram->SetUniform
					("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);
			}

			renderer->GetMesh()->Draw(m_skinnedDepthProgram.get());
		}
	}

	glCullFace(GL_BACK);
}

void ShadowDepthRenderPass::CalculateLightSpaceMatrix(Scene* scene)
{
	// 1. 메인 조명 하나를 가져온다.
	SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());
	if (!mainLight) return;

	// 2. 조명 시점의 View/Projection 행렬 계산 (Light Space Matrix)
	auto& lightTransform = mainLight->GetTransform();
	glm::vec3 lightPos = lightTransform.GetPosition();
	glm::vec3 lightDir = lightTransform.GetForwardVector();

	// 3. 조명을 기준으로하는 투영 행렬을 계산
	glm::vec2 cutoff = mainLight->GetCutoff();
	glm::vec3 up	 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, up);
	glm::mat4 lightProj = glm::perspective(glm::radians((cutoff[0] + cutoff[1]) * 2.0f), 1.0f, 1.0f, 100.0f);
	m_lightSpaceMatrix = lightProj * lightView;
}
