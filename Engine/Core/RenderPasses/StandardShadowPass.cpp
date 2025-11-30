#include "EnginePch.h"
#include "StandardShadowPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Core/StandardRenderPipeline.h"
#include "Core/RenderPasses/StandardGeometryPass.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"     
#include "Graphics/ShadowMap.h"
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"

StandardShadowPassUPtr StandardShadowPass::Create(int32 resolution)
{
	auto pass = StandardShadowPassUPtr(new StandardShadowPass());
	if (!pass->Init(resolution)) return nullptr;
	return std::move(pass);
}

bool StandardShadowPass::Init(int32 resolution)
{
	m_resolution = resolution;

	// TODO : 프로그램은 여기서 생성!
	m_staticDepthProgram = Program::Create
	(
		"./Resources/Shaders/Shadow/static_shadow.vert",
		"./Resources/Shaders/Shadow/static_shadow.frag"
	);
	m_skinnedDepthProgram = Program::Create
	(
		"./Resources/Shaders/Shadow/skinned_shadow.vert",
		"./Resources/Shaders/Shadow/skinned_shadow.frag"
	);
	if (!m_staticDepthProgram || !m_skinnedDepthProgram)
		return false;

	m_shadowMaps.resize(MAX_SHADOW_CASTER);
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		m_shadowMaps[i] = ShadowMap::Create(resolution, resolution);
		if (!m_shadowMaps[i])
		{
			SPDLOG_ERROR("Failed to create ShadowMap index {}", i);
			return false;
		}
	}

	return true;
}

// TODO : Render 추상 메서드 생김새를 조금 다듬을 필요는 있음
void StandardShadowPass::Render(Scene* scene, Camera* camera)
{
	// 1. 공통 설정
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	// 2. 전체 조명 리스트 순회
	auto pipeline = (StandardRenderPipeline*)(RENDER.GetRenderer()->GetPipeline());
	for (auto* light : scene->GetLights())
	{
		// 2-1. 그림자 캐스팅이 필요한 조명인지 검사
		if (!light->IsCastShadow()) continue;

		// 2-2. 그림자 맵 인덱스 검사
		int32 shadowIdx = light->GetShadowMapIndex();
		if (shadowIdx >= MAX_SHADOW_CASTER) continue;

		// 2-3. 그림자 맵 바인딩
		auto& currentShadowMap = m_shadowMaps[shadowIdx];
		currentShadowMap->Bind();
		glViewport(0, 0, m_resolution, m_resolution);
		glClear(GL_DEPTH_BUFFER_BIT);

		// 2-4. 조명 행렬 계산
		glm::mat4 lightSpaceMatrix = CalculateLightSpaceMatrix(light);
		light->SetLightSpaceMatrix(lightSpaceMatrix);

		// 2-5. StaticMesh 렌더링
		if (m_staticDepthProgram)
		{
			m_staticDepthProgram->Use();
			m_staticDepthProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

			const auto& staticRenderers = pipeline->GetGeometryPass()->GetStaticMeshRenderers();
			for (const auto* renderer : staticRenderers)
			{
				auto model = renderer->GetTransform().GetModelMatrix();
				m_staticDepthProgram->SetUniform("model", model);
				renderer->GetMesh()->Draw(m_staticDepthProgram.get());
			}
		}

		// 2-6. SkinnedMesh 렌더링
		if (m_skinnedDepthProgram)
		{
			m_skinnedDepthProgram->Use();
			m_skinnedDepthProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

			// G-Buffer에서 그릴 대상 중 SkinnedMesh를 가져옴
			const auto& skinnedRenderers = pipeline->GetGeometryPass()->GetSkinnedMeshRenderers();
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
	}

	glCullFace(GL_BACK);
}

glm::mat4 StandardShadowPass::CalculateLightSpaceMatrix(Light* light)
{
	if (!light) return glm::mat4(1.0f);

	// 행렬 연산에 필요한 것들을 가져오기
	auto& transform = light->GetTransform();
	glm::vec3 pos = transform.GetPosition();
	glm::vec3 dir = transform.GetForwardVector();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightProjection;
	glm::mat4 lightView = glm::lookAt(pos, pos + dir, up);

	// 조명의 타입 별로 조명 기준의 projection, view 행렬을 계산
	float size = 20.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	switch (light->GetLightType())
	{
	case LightType::Directional:
		lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);
		break;

	case LightType::Spot:
		glm::vec2 cutoff = static_cast<SpotLight*>(light)->GetCutoff();
		lightProjection = glm::perspective(glm::radians((cutoff[0] + cutoff[1]) * 2.0f),
			1.0f, 1.0f, 100.0f);
		break;
	}

	// 조명 기준의 VP 행렬 반환
	return lightProjection * lightView;
}