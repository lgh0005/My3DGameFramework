#include "EnginePch.h"
#include "ShadowPass.h"

#include "Scene/Scene.h"
#include "Object/GameObject.h"
#include "Resources/Program.h" 
#include "Graphics/ShadowMap.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Textures/Texture.h" 
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Animator.h"

DECLARE_DEFAULTS_IMPL(ShadowPass)

ShadowPassUPtr ShadowPass::Create(int32 resolution)
{
	auto pass = ShadowPassUPtr(new ShadowPass());
	if (!pass->Init(resolution)) return nullptr;
	return std::move(pass);
}

bool ShadowPass::Init(int32 resolution)
{
	// 0. 그림자의 해상도 설정 : 기본값 1024
	m_resolution = resolution;

	// 1. 그림자 셰이더 프로그램 생성
	m_staticDepthProgram = RESOURCE.GetResource<Program>("common_shadow_depth_static");
	m_skinnedDepthProgram = RESOURCE.GetResource<Program>("common_shadow_depth_skinned");

	// 2. 그림자가 드리워지는 최대 조명 개수 만큼 그림자 맵 생성
	// 최대(MAX_SHADOW_CASTER) 8개
	m_shadowMaps.resize(MAX_SHADOW_CASTER);
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		m_shadowMaps[i] = ShadowMap::Create(resolution);
		if (!m_shadowMaps[i])
		{
			LOG_ERROR("Failed to create ShadowMap index {}", i);
			return false;
		}
	}

	return true;
}

void ShadowPass::Render(RenderContext* context)
{
	// 0. Context 유효성 검사 및 필요 정보 가져오기
	if (!context) return;
	auto lights = context->GetLights();
	auto staticMeshes = context->GetStaticMeshRenderers();
	auto skinnedMeshes = context->GetSkinnedMeshRenderers();

	// 1. 공통 설정
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	// 2. 전체 조명 리스트 순회
	for (auto* light : lights)
	{
		// 2-0. 조명 자체가 꺼져있거나, 조명의 주인이 비활성 상태면 패스
		// (꺼진 조명은 그림자도 만들지 않아야 함)
		if (!light->IsEnabled()) continue;
		if (!light->GetOwner()->IsActive()) continue;

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
		RenderStaticMeshes(staticMeshes, lightSpaceMatrix);

		// 2-6. SkinnedMesh 렌더링
		RenderSkinnedMeshes(skinnedMeshes, lightSpaceMatrix);
	}

	glCullFace(GL_BACK);

	// 3. 그림자를 구운 텍스쳐를 Context에 등록
	RegisterShadowMapsToContext(context);
}

glm::mat4 ShadowPass::CalculateLightSpaceMatrix(Light* light)
{
	if (!light) return glm::mat4(1.0f);

	// 행렬 연산에 필요한 것들을 가져오기
	auto& transform = light->GetTransform();
	glm::vec3 pos = transform.GetWorldPosition();
	glm::vec3 dir = transform.GetForwardVector();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightProjection;
	glm::mat4 lightView = glm::lookAt(pos, pos + dir, up);

	// 조명의 타입 별로 조명 기준의 projection, view 행렬을 계산
	float size = 20.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	switch (light->GetComponentType())
	{
		case ComponentType::DirectionalLight:
			lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);
			break;

		case ComponentType::SpotLight:
			glm::vec2 cutoff = static_cast<SpotLight*>(light)->GetCutoff();
			lightProjection = glm::perspective(glm::radians((cutoff[0] + cutoff[1]) * 2.0f),
				1.0f, 1.0f, 100.0f);
			break;

		// TODO : Point Light에 대한 그림자 처리는 이후에 고려 필요
	}

	// 조명 기준의 VP 행렬 반환
	return lightProjection * lightView;
}

void ShadowPass::RenderStaticMeshes
(
	const std::vector<StaticMeshRenderer*>& meshes,
	const glm::mat4& lightSpaceMatrix
)
{
	if (!m_staticDepthProgram || meshes.empty()) return;

	m_staticDepthProgram->Use();
	m_staticDepthProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

	for (const auto* renderer : meshes)
	{
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActive()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_staticDepthProgram->SetUniform("model", model);
		renderer->Render(m_staticDepthProgram.get());
	}
}

void ShadowPass::RenderSkinnedMeshes
(
	const std::vector<SkinnedMeshRenderer*>& meshes,
	const glm::mat4& lightSpaceMatrix
)
{
	if (!m_skinnedDepthProgram || meshes.empty()) return;

	m_skinnedDepthProgram->Use();
	m_skinnedDepthProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

	for (const auto* renderer : meshes)
	{
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActive()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_skinnedDepthProgram->SetUniform("model", model);

		Animator* animator = renderer->GetAnimator();
		if (animator) m_skinnedDepthProgram->SetUniform("finalBoneMatrices", animator->GetFinalBoneMatrices());
		else m_skinnedDepthProgram->SetUniform("finalBoneMatrices", GetIdentityBones());

		renderer->Render(m_skinnedDepthProgram.get());
	}
}

void ShadowPass::RegisterShadowMapsToContext(RenderContext* context)
{
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		if (m_shadowMaps[i])
		{
			Texture* tex = m_shadowMaps[i]->GetShadowMap().get();
			context->SetShadowMap(i, tex);
		}
		else
		{
			context->SetShadowMap(i, nullptr);
		}
	}
}

void ShadowPass::Resize(int32 resolution)
{
	if (m_resolution == resolution) return;

	m_resolution = resolution;
	for (auto& shadowMap : m_shadowMaps)
	{
		if (shadowMap)
			shadowMap->Resize(m_resolution);
	}
}