#include "EnginePch.h"
#include "UniversalDeferredLightingPass.h"

#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/SRP/StandardRenderPipeline.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
#include "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"

#include "Scene/Scene.h"
#include "Scene/SceneRegistry.h"
#include "Graphics/Geometry.h"
#include "Resources/Program.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h" 
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/SkyLight.h"
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Framebuffers/PostProcessFramebuffer.h"

#include "Pipelines/URP/UniversalRenderContext.h"

DECLARE_DEFAULTS_IMPL(UniversalDeferredLightingPass)

UniversalDeferredLightingPassUPtr UniversalDeferredLightingPass::Create()
{
	auto pass = UniversalDeferredLightingPassUPtr(new UniversalDeferredLightingPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool UniversalDeferredLightingPass::Init()
{
	m_deferredLightProgram = Program::Create
	(
		"./Resources/Shaders/Universal/Universal_Deferred_LightPass.vert",
		"./Resources/Shaders/Universal/Universal_Deferred_LightPass.frag"
	);
	if (!m_deferredLightProgram) return false;

	m_plane = ScreenMesh::Create();
	if (!m_plane) return false;

	return true;
}

void UniversalDeferredLightingPass::Render(RenderContext* context)
{
	// 0. Context 캐스팅 및 유효성 검사
	auto stdCtx = (UniversalRenderContext*)context;
	if (!stdCtx) return;

	// 1. 포스트 프로세싱 프레임 버퍼에 GBuffer 내용 그리기
	BeginDrawOnPostProcessFBO(stdCtx);

	// 2. SSAO 텍스쳐 바인딩
	BindSSAOTexture(stdCtx);

	// 3. 그림자 맵 바인딩
	BindShadowMaps(stdCtx);

	// 4. SkyLight으로부터 IBL 속성 바인딩
	BindIBLMaps(stdCtx);

	// 4. RenderContext로부터 조명 행렬 가져오기
	GetLightMatricesFromContext(stdCtx);

	// 6. 그리기
	m_plane->Draw();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

/*==========================================//
//   deferred lighting pass helper methods  //
//==========================================*/
void UniversalDeferredLightingPass::BeginDrawOnPostProcessFBO(UniversalRenderContext* context)
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

void UniversalDeferredLightingPass::BindSSAOTexture(UniversalRenderContext* context)
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

void UniversalDeferredLightingPass::BindShadowMaps(UniversalRenderContext* context)
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

void UniversalDeferredLightingPass::BindIBLMaps(UniversalRenderContext* context)
{
	// 6. IBL (SkyLight) Binding [Slot 13-15]
	SkyLight* skyLight = context->GetSkyLight();
	m_deferredLightProgram->SetUniform("useIBL", true);
	if (skyLight)
	{
		// 13. Irradiance
		auto* irradiance = skyLight->GetIrradianceMap();
		if (irradiance)
		{
			glActiveTexture(GL_TEXTURE13);
			irradiance->Bind();
			m_deferredLightProgram->SetUniform("irradianceMap", 13);
		}

		// 14. Prefilter
		auto* prefilter = skyLight->GetPrefilterMap();
		if (prefilter)
		{
			glActiveTexture(GL_TEXTURE14);
			prefilter->Bind();
			m_deferredLightProgram->SetUniform("prefilterMap", 14);
		}

		// 15. BRDF LUT
		auto* brdf = skyLight->GetBRDFLookUp();
		if (brdf)
		{
			glActiveTexture(GL_TEXTURE15);
			brdf->Bind();
			m_deferredLightProgram->SetUniform("brdfLUT", 15);
		}
	}
}

void UniversalDeferredLightingPass::GetLightMatricesFromContext(UniversalRenderContext* context)
{
	// 6. Light Matrices 전송(Context의 Culled List 사용)
	// Scene 전체 조명이 아니라, Context에 담긴 조명만 처리
	// TODO : 배열 쨰로 넘기게 수정 필요
	const auto& lights = context->GetSceneRegistry()->GetLights();
	for (auto* light : lights)
	{
		int32 idx = light->GetShadowMapIndex();
		if (idx >= 0 && idx < MAX_SHADOW_CASTER)
		{
			std::string uName = "lightSpaceMatrices[" + std::to_string(idx) + "]";
			m_deferredLightProgram->SetUniform(uName, light->GetLightSpaceMatrix());
		}
	}
}