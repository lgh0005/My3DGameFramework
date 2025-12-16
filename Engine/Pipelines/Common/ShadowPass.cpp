#include "EnginePch.h"
#include "ShadowPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Pipelines/SRP/StandardRenderPipeline.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
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

#include "Pipelines/SRP/StandardRenderContext.h"

ShadowPassUPtr ShadowPass::Create(int32 resolution)
{
	auto pass = ShadowPassUPtr(new ShadowPass());
	if (!pass->Init(resolution)) return nullptr;
	return std::move(pass);
}

bool ShadowPass::Init(int32 resolution)
{
	m_resolution = resolution;

	m_staticDepthProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Shadow_DepthPass_Static.vert",
		"./Resources/Shaders/Common/Common_Shadow_DepthPass_Static.frag"
	);
	m_skinnedDepthProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Shadow_DepthPass_Skinned.vert",
		"./Resources/Shaders/Common/Common_Shadow_DepthPass_Skinned.frag"
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

// TEST : Context에 있는 내용물을 잘 렌더링 하는 지 테스트
void ShadowPass::Render(RenderContext* context)
{
	// 0. Context 캐스팅 및 유효성 검사
	auto stdCtx = (StandardRenderContext*)context;
	auto lights = stdCtx->GetLights();
	auto staticMeshes = stdCtx->GetStaticMeshRenderers();
	auto skinnedMeshes = stdCtx->GetSkinnedMeshRenderers();
	if (!stdCtx) return;

	// 1. 공통 설정
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	// 2. 전체 조명 리스트 순회
	for (auto* light : lights)
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

			for (const auto* renderer : staticMeshes)
			{
				auto model = renderer->GetTransform().GetWorldMatrix();
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
			for (const auto* renderer : skinnedMeshes)
			{
				GameObject* go = renderer->GetOwner();
				auto model = renderer->GetTransform().GetWorldMatrix();
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

	// 3. 그림자를 구운 텍스쳐를 Context에 등록
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		if (m_shadowMaps[i])
		{
			// ShadowMap 클래스 내부의 Texture를 가져옵니다.
			Texture* tex = m_shadowMaps[i]->GetShadowMap().get();
			stdCtx->SetShadowMap(i, tex);
		}
		else
		{
			stdCtx->SetShadowMap(i, nullptr);
		}
	}
}

glm::mat4 ShadowPass::CalculateLightSpaceMatrix(Light* light)
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