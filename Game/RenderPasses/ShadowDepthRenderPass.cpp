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
		const auto& staticRenderers = scene->GetGeometryPass()->GetRenderers();
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
		const auto& skinnedRenderers = scene->GetGeometryPass()->GetSkinnedMeshRenderers();
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
	//// 1. 메인 조명 하나를 가져온다.
	//SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());
	//if (!mainLight) return;

	//// 2. 조명 시점의 View/Projection 행렬 계산 (Light Space Matrix)
	//auto& lightTransform = mainLight->GetTransform();
	//glm::vec3 lightPos = lightTransform.GetPosition();
	//glm::vec3 lightDir = lightTransform.GetForwardVector();

	//// 3. 조명을 기준으로하는 투영 행렬을 계산
	//glm::vec2 cutoff = mainLight->GetCutoff();
	//glm::vec3 up	 = glm::vec3(0.0f, 1.0f, 0.0f);
	//glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, up);
	//glm::mat4 lightProj = glm::perspective(glm::radians((cutoff[0] + cutoff[1]) * 2.0f), 1.0f, 1.0f, 100.0f);
	//m_lightSpaceMatrix = lightProj * lightView;

	// 1. 메인 조명 가져오기 (Light* 기본 클래스로 받음)
	Light* mainLight = scene->GetMainLight();
	if (!mainLight) return;

	auto& transform = mainLight->GetTransform();
	glm::vec3 pos = transform.GetPosition();
	glm::vec3 dir = transform.GetForwardVector();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 lightProjection;
	glm::mat4 lightView = glm::lookAt(pos, pos + dir, up);

	float size = 20.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	switch (mainLight->GetLightType())
	{
	case LightType::Directional:
		lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);
		break;

	case LightType::Spot:
		glm::vec2 cutoff = static_cast<SpotLight*>(mainLight)->GetCutoff();
		lightProjection = glm::perspective(glm::radians((cutoff[0] + cutoff[1]) * 2.0f), 1.0f, 1.0f, 100.0f);
		break;

	}

	//// 2. 타입에 따른 분기
	//if (mainLight->GetLightType() == LightType::Directional)
	//{
	//	// [Directional Light] -> 직교 투영 (Ortho)
	//	// 태양 그림자는 "박스" 안에 있는 것만 그립니다.
	//	// size가 너무 작으면 그림자가 잘리고, 너무 크면 해상도가 떨어집니다. (일단 20~50 정도 추천)
	//	float size = 20.0f;
	//	float nearPlane = 0.1f;
	//	float farPlane = 100.0f;

	//	// (좌, 우, 하, 상, 근, 원)
	//	lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);

	//	// View 행렬: 조명 위치에서 바라보는 방향
	//	// Directional은 위치가 상관없지만, 쉐도우 맵을 찍으려면 
	//	// 씬을 감싸는 적절한 위치(pos)에 조명 오브젝트가 있어야 합니다.
	//	lightView = glm::lookAt(pos, pos + dir, up);
	//}
	//else if (mainLight->GetLightType() == LightType::Spot)
	//{
	//	// [Spot Light] -> 원근 투영 (Perspective)
	//	// 기존 코드 유지 (안전하게 캐스팅)
	//	auto spot = static_cast<SpotLight*>(mainLight);

	//	glm::vec2 cutoff = spot->GetCutoff();
	//	// FOV는 Outer Cutoff 각도의 2배
	//	float fov = cutoff[1] * 2.0f;
	//	float aspect = 1.0f; // 텍스처가 정사각형이므로
	//	float nearPlane = 0.1f;
	//	float farPlane = spot->GetDistance(); // 조명 사거리만큼

	//	lightProjection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
	//	lightView = glm::lookAt(pos, pos + dir, up);
	//}

	// 3. 결과 저장
	m_lightSpaceMatrix = lightProjection * lightView;
}
