#include "EnginePch.h"
#include "StandardDeferredLightingPass.h"

#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/SRP/StandardRenderPipeline.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
#include "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"

#include "Scene/SceneRegistry.h"
#include "GameObject/GameObject.h"
#include "Graphics/Geometry.h"
#include "Resources/Program.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h" 
#include "Resources/Texture.h" 
#include "Graphics/ShadowMap.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Framebuffers/PostProcessFramebuffer.h"

#include "Pipelines/SRP/StandardRenderContext.h"

DECLARE_DEFAULTS_IMPL(StandardDeferredLightingPass)

StandardDeferredLightingPassUPtr StandardDeferredLightingPass::Create()
{
	auto pass = StandardDeferredLightingPassUPtr(new StandardDeferredLightingPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool StandardDeferredLightingPass::Init()
{
	m_deferredLightProgram = RESOURCE.GetResource<Program>("standard_deferred_lighting");
	m_plane = RESOURCE.GetResource<ScreenMesh>("Screen");
	return true;
}

void StandardDeferredLightingPass::Render(RenderContext* context)
{
	// 0. Context 캐스팅 및 유효성 검사
	auto stdCtx = (StandardRenderContext*)context;
	if (!stdCtx) return;

	// 1. 포스트 프로세싱 프레임 버퍼에 GBuffer 내용 그리기
	BeginDrawOnPostProcessFBO(stdCtx);

	// 2. SSAO 텍스쳐 바인딩
	BindSSAOTexture(stdCtx);

	// 3. 그림자 맵 바인딩
	BindShadowMaps(stdCtx);

	// 4. RenderContext로부터 조명 행렬 가져오기
	GetLightMatricesFromContext(stdCtx);

	// 5. 그리기
	m_plane->Draw();
	glEnable(GL_DEPTH_TEST);
}

/*==========================================//
//   deferred lighting pass helper methods  //
//==========================================*/
void StandardDeferredLightingPass::BeginDrawOnPostProcessFBO(StandardRenderContext* context)
{
	// 1. Context에서 G-Buffer 텍스처 가져오기
	Texture* tPos = context->GetGBufferPosition();
	Texture* tNormal = context->GetGBufferNormal();
	Texture* tAlbedo = context->GetGBufferAlbedo();
	Texture* tEmission = context->GetGBufferEmission();
	if (!tPos || !tNormal || !tAlbedo || !tEmission) return;

	// 2. 그리기 준비 (Output FBO 설정)
	PostProcessFramebuffer* targetFBO = context->GetTargetFramebuffer();
	if (targetFBO)
	{
		targetFBO->Bind();
		glViewport(0, 0, targetFBO->GetWidth(), targetFBO->GetHeight());
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else
	{
		Framebuffer::BindToDefault();
		// 텍스처 크기를 기준으로 뷰포트 설정
		glViewport(0, 0, tPos->GetWidth(), tPos->GetHeight());
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	m_deferredLightProgram->Use();

	// 3. G-Buffer 텍스처 바인딩 (Context 데이터 사용)
	glActiveTexture(GL_TEXTURE0); tPos->Bind();
	m_deferredLightProgram->SetUniform("gPosition", 0);

	glActiveTexture(GL_TEXTURE1); tNormal->Bind();
	m_deferredLightProgram->SetUniform("gNormal", 1);

	glActiveTexture(GL_TEXTURE2); tAlbedo->Bind();
	m_deferredLightProgram->SetUniform("gAlbedoSpec", 2);

	glActiveTexture(GL_TEXTURE3); tEmission->Bind();
	m_deferredLightProgram->SetUniform("gEmission", 3);
}

void StandardDeferredLightingPass::BindSSAOTexture(StandardRenderContext* context)
{
	// 4. SSAO 텍스처 바인딩 (Context 데이터 사용)
	Texture* tSSAO = context->GetSSAOTexture();
	if (tSSAO)
	{
		glActiveTexture(GL_TEXTURE12);
		tSSAO->Bind();
		m_deferredLightProgram->SetUniform("ssaoTexture", 12);
		m_deferredLightProgram->SetUniform("useSSAO", true);
	}
	else
	{
		m_deferredLightProgram->SetUniform("useSSAO", false);
	}
}

void StandardDeferredLightingPass::BindShadowMaps(StandardRenderContext* context)
{
	// 5. Shadow Maps 바인딩 (Pipeline 데이터 사용 - 기존 유지)
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		glActiveTexture(GL_TEXTURE4 + i);

		Texture* shadowMap = context->GetShadowMap(i);
		if (shadowMap) shadowMap->Bind();

		std::string uniformName = "shadowMaps[" + std::to_string(i) + "]";
		m_deferredLightProgram->SetUniform(uniformName, 4 + i);
	}
}

void StandardDeferredLightingPass::GetLightMatricesFromContext(StandardRenderContext* context)
{
	// 6. Light Matrices 전송(Context의 Culled List 사용)

	// Scene 전체 조명이 아니라, Context에 담긴 조명만 처리
	// TODO : 이 부분은 SetUniform(const std::string& name, const std::vector<glm::mat4>& value)
	// 을 이용한다.

	std::vector<glm::mat4> lightSpaceMatrices(MAX_SHADOW_CASTER, glm::mat4(1.0f));

	const auto& lights = context->GetSceneRegistry()->GetLights();
	for (auto* light : lights)
	{
		if (!light->IsEnabled()) continue;
		if (!light->GetOwner()->IsActiveInHierarchy()) continue;

		int32 idx = light->GetShadowMapIndex();
		if (idx >= 0 && idx < MAX_SHADOW_CASTER)
			lightSpaceMatrices[idx] = light->GetLightSpaceMatrix();
	}

	m_deferredLightProgram->SetUniform("lightSpaceMatrices", lightSpaceMatrices);
}