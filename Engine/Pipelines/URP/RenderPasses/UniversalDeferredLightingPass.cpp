#include "EnginePch.h"
#include "UniversalDeferredLightingPass.h"

#include "Pipelines/Common/ShadowPass.h"

#include "Scene/ComponentRegistry.h"
#include "Object/GameObject.h"
#include "Scene/SceneRegistry.h"
#include "Graphics/Geometry.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Meshes/StaticMesh.h" 
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Components/SkyLight.h"
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"

DECLARE_DEFAULTS_IMPL(UniversalDeferredLightingPass)

UniversalDeferredLightingPassUPtr UniversalDeferredLightingPass::Create()
{
	auto pass = UniversalDeferredLightingPassUPtr(new UniversalDeferredLightingPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool UniversalDeferredLightingPass::Init()
{
	m_deferredLightProgram = RESOURCE.GetResource<Program>("universal_deferred_lighting");
	m_plane = RESOURCE.GetResource<ScreenMesh>("Screen");
	if (!m_deferredLightProgram || !m_plane) return false;

	// 1. 변하지 않는 샘플러 인덱스는 초기화 시점에 한 번만 전송
	m_deferredLightProgram->Use();

	// 1-1. G-Buffer 텍스쳐 유니폼 설정
	m_deferredLightProgram->SetUniform("gPosition", 0);
	m_deferredLightProgram->SetUniform("gNormal", 1);
	m_deferredLightProgram->SetUniform("gAlbedoSpec", 2);
	m_deferredLightProgram->SetUniform("gEmission", 3);

	// 1-2. IBL 유니폼 설정
	// INFO : 이거 여기에 유니폼 설정하면 동작이 안될 수도 있음. 경험상.
	m_deferredLightProgram->SetUniform("useIBL", true);
	m_deferredLightProgram->SetUniform("irradianceMap", 13);
	m_deferredLightProgram->SetUniform("prefilterMap", 14);
	m_deferredLightProgram->SetUniform("brdfLUT", 15);

	// 1-2. 그림자 맵 유니폼 설정
	std::vector<int> shadowIndices(MAX_SHADOW_CASTER);
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i) shadowIndices[i] = 4 + i;
	m_deferredLightProgram->SetUniform("shadowMaps", shadowIndices);

	// 1-3. SSAO 텍스쳐 유니폼 설정
	m_deferredLightProgram->SetUniform("ssaoTexture", 12);

	return true;
}

void UniversalDeferredLightingPass::Render(RenderContext* context)
{
	// 0. Context 캐스팅 및 유효성 검사
	if (!context) return;

	// 1. 포스트 프로세싱 프레임 버퍼에 GBuffer 내용 그리기
	BeginDrawOnPostProcessFBO(context);

	// 2. SSAO 텍스쳐 바인딩
	BindSSAOTexture(context);

	// 3. 그림자 맵 바인딩
	BindShadowMaps(context);

	// 4. SkyLight으로부터 IBL 속성 바인딩
	BindIBLMaps(context);

	// 4. RenderContext로부터 조명 행렬 가져오기
	GetLightMatricesFromContext(context);

	// 6. 그리기
	glDisable(GL_DEPTH_TEST);
	m_plane->Draw();
	glEnable(GL_DEPTH_TEST);

	// 7. 사용한 FBO 해제
	Framebuffer::BindToDefault();
}

/*==========================================//
//   deferred lighting pass helper methods  //
//==========================================*/
void UniversalDeferredLightingPass::BeginDrawOnPostProcessFBO(RenderContext* context)
{
	// 1. Context에서 G-Buffer 텍스처 가져오기
	Texture* tPos = context->GetTexture(RenderSlot::GPosition);
	Texture* tNormal = context->GetTexture(RenderSlot::GNormal);
	Texture* tAlbedo = context->GetTexture(RenderSlot::GAlbedo);
	Texture* tEmission = context->GetTexture(RenderSlot::GEmission);
	if (!tPos || !tNormal || !tAlbedo || !tEmission) return;

	// 2. 그리기 준비 (Output FBO 설정)
	context->BindTargetFramebuffer();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 3. G-Buffer 텍스처 바인딩 (Context 데이터 사용)
	m_deferredLightProgram->Use();
	glActiveTexture(GL_TEXTURE0); tPos->Bind();
	glActiveTexture(GL_TEXTURE1); tNormal->Bind();
	glActiveTexture(GL_TEXTURE2); tAlbedo->Bind();
	glActiveTexture(GL_TEXTURE3); tEmission->Bind();
}

void UniversalDeferredLightingPass::BindSSAOTexture(RenderContext* context)
{
	// 4. SSAO 텍스처 바인딩 (Context 데이터 사용)
	Texture* tSSAO = context->GetTexture(RenderSlot::SSAO);
	glActiveTexture(GL_TEXTURE12);

	if (tSSAO)
	{
		tSSAO->Bind();
		m_deferredLightProgram->SetUniform("useSSAO", true);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		m_deferredLightProgram->SetUniform("useSSAO", false);
	}
}

void UniversalDeferredLightingPass::BindShadowMaps(RenderContext* context)
{
	// 5. Shadow Maps 바인딩 (Pipeline 데이터 사용 - 기존 유지)
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		glActiveTexture(GL_TEXTURE4 + i);

		Texture* shadowMap = context->GetShadowMap(i);
		if (shadowMap) shadowMap->Bind();
		else glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void UniversalDeferredLightingPass::BindIBLMaps(RenderContext* context)
{
	// 6. IBL (SkyLight) Binding [Slot 13-15]
	SkyLight* skyLight = context->GetSkyLight();
	if (skyLight)
	{
		// 13. Irradiance
		auto* irradiance = skyLight->GetIrradianceMap();
		if (irradiance)
		{
			glActiveTexture(GL_TEXTURE13);
			irradiance->Bind();
		}

		// 14. Prefilter
		auto* prefilter = skyLight->GetPrefilterMap();
		if (prefilter)
		{
			glActiveTexture(GL_TEXTURE14);
			prefilter->Bind();
		}

		// 15. BRDF LUT
		auto* brdf = skyLight->GetBRDFLookUp();
		if (brdf)
		{
			glActiveTexture(GL_TEXTURE15);
			brdf->Bind();
		}
	}
}

void UniversalDeferredLightingPass::GetLightMatricesFromContext(RenderContext* context)
{
	// 6. Light Matrices 전송
	static std::vector<glm::mat4> lightSpaceMatrices(MAX_SHADOW_CASTER, glm::mat4(1.0f));
	std::fill(lightSpaceMatrices.begin(), lightSpaceMatrices.end(), glm::mat4(1.0f));

	// 8. 유효한 조명 가져오기
	const auto& lights = context->GetLights();
	for (Light* light : lights)
	{
		if (!light->IsEnabled()) continue;
		if (!light->GetOwner()->IsActive()) continue;

		int32 idx = light->GetShadowMapIndex();
		if (idx >= 0 && idx < MAX_SHADOW_CASTER)
			lightSpaceMatrices[idx] = light->GetLightSpaceMatrix();
	}

	m_deferredLightProgram->SetUniform("lightSpaceMatrices", lightSpaceMatrices);
}