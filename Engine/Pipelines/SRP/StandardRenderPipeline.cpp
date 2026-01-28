#include "EnginePch.h"
#include "StandardRenderPipeline.h"

#include "Pipelines/Common/CullingPass.h"
#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/Common/SkyboxPass.h"
#include "Pipelines/Common/JoltDebugGizmoPass.h"
#include "Pipelines/Common/PostProcessPass.h"
#include "Pipelines/Common/SSAOPass.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
#include "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"
#include "Pipelines/SRP/StandardGlobalUniforms.h"

#include "Scene/Scene.h"
#include "Scene/ComponentRegistry.h"
#include "Object/GameObject.h"
#include "Resources/Programs/Program.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Meshes/StaticMesh.h" 
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"
#include "Graphics/Framebuffers/PostProcessFramebuffer.h"
#include "Graphics/Framebuffers/GBufferFramebuffer.h"

#include "Pipelines/PostProcessing/World/MotionBlurEffect.h"
#include "Pipelines/PostProcessing/World/OutlineEffect.h"
#include "Pipelines/PostProcessing/World/WorldSpaceEffects.h"
#include "Pipelines/PostProcessing/Bloom/DOFEffect.h"
#include "Pipelines/PostProcessing/Bloom/GaussianBloomEffect.h"
#include "Pipelines/PostProcessing/Bloom/LensFlareEffect.h"
#include "Pipelines/PostProcessing/Display/DisplayMappingEffects.h"

DECLARE_DEFAULTS_IMPL(StandardRenderPipeline)

StandardRenderPipelineUPtr StandardRenderPipeline::Create()
{
	auto renderPipeline = StandardRenderPipelineUPtr(new StandardRenderPipeline());
	if (!renderPipeline->Init()) return nullptr;
	return std::move(renderPipeline);
}

bool StandardRenderPipeline::Init()
{
	// UBO 생성
	m_globalUniforms = StandardGlobalUniforms::Create();
	if (!m_globalUniforms) return false;

	// 컬링 패스 생성
	m_cullingPass = CullingPass::Create();
	if (!m_cullingPass) return false;

	// 셰도우 패스 생성
	m_shadowPass = ShadowPass::Create();
	if (!m_shadowPass) return false;

	// 스카이박스 패스 생성
	m_skyboxPass = SkyboxPass::Create();
	if (!m_skyboxPass) return false;

	// 디버그 패스 생성
	m_debugGizmoPass = JoltDebugGizmoPass::Create();
	if (!m_debugGizmoPass) return false;

	// SSAO 패스 생성
	m_ssaoPass = SSAOPass::Create();
	if (!m_ssaoPass) return false;

	// 통용 포스트-프로세스 패스 생성
	m_postProcessPass = PostProcessPass::Create();
	if (!m_postProcessPass) return false;
	auto motion = MotionBlurEffect::Create(0);
	m_postProcessPass->AddEffect(std::move(motion));
	auto world = WorldSpaceEffects::Create(1);
	m_postProcessPass->AddEffect(std::move(world));
	auto outline = OutlineEffect::Create(3);
	m_postProcessPass->AddEffect(std::move(outline));
	auto bloom = GaussianBloomEffect::Create(4);
	m_postProcessPass->AddEffect(std::move(bloom));
	auto display = DisplayMappingEffects::Create(6);
	display->SetToneMappingMode(ToneMappingMode::Exposure); // 0번 모드
	display->SetBloomStrength(1.0f);
	display->SetExposure(1.0f);
	display->SetUseCRT(false);
	m_postProcessPass->AddEffect(std::move(display));

	// G-buffer 패스 생성
	m_geometryPass = StandardGeometryPass::Create();
	if (!m_geometryPass) return false;

	// Light 패스 생성
	m_deferredLightPass = StandardDeferredLightingPass::Create();
	if (!m_deferredLightPass) return false;

	return true;
}

void StandardRenderPipeline::Render(Scene* scene)
{
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	// TODO : 이후에는 여러 카메라를 지원할 것이라면 카메라 개수 만큼
	// 반복해야 할 수도 있음.
	auto* registry = scene->GetComponentRegistry();
	auto* camera = registry->GetCamera(0);
	if (!camera) return;

	// 0. 스택 영역에 StandardRenderContext 생성
	RenderContext context;
	context.Reset(scene, camera);

	// [패스 0~1] 컬링 및 그림자
	m_cullingPass->Render(&context);
	m_globalUniforms->PreRender(&context);
	m_shadowPass->Render(&context);

	// [패스 2] 디퍼드 지오메트리 (G-Buffer 채우기)
	m_geometryPass->Render(&context);

	// [패스 3] SSAO (G-Buffer 읽어서 Occlusion 계산)
	m_ssaoPass->Render(&context);

	// [패스 4] 디퍼드 라이팅 (G-Buffer + SSAO + Shadow -> PostProcessFBO)
	context.SetTargetFramebuffer(m_postProcessPass->GetFramebuffer());
	m_deferredLightPass->Render(&context);
	Framebuffer::Blit
	(
		m_geometryPass->GetGBuffer(),m_postProcessPass->GetFramebuffer(),
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);

	// [패스 5] 포워드 셰이딩
	context.BindTargetFramebuffer();
	for (const auto& [name, pass] : registry->GetCustomRenderPasses())
		pass->Render(scene, camera);

	// [패스 6] 스카이박스 패스
	m_skyboxPass->Render(&context);

	// [패스 7] 포스트 프로세싱 패스 (최종 합성 및 화면 출력)
	m_postProcessPass->Render(&context);

	// [패스 8] 디버그 패스 (ImGUI 컨텍스트와 충돌 기즈모 출력)
	// m_debugGizmoPass->Render(&context);
	RenderIMGUIContext();
}

void StandardRenderPipeline::OnResize(int32 width, int32 height)
{
	// 모든 패스의 FBO 내부 텍스처 갱신
	m_geometryPass->Resize(width, height);
	m_postProcessPass->Resize(width, height);
	m_ssaoPass->Resize(width, height);
}

/*======================//
//  debug imgui method  //
//======================*/
#pragma region IMGUI_DEBUG_METHOD

void StandardRenderPipeline::RenderIMGUIContext()
{
	IMGUI.BeginFrame();

	if (IMGUI.Begin("Test Window"))
	{
		ImGui::Text("Default Render Pipeline #1");
		ImGui::Text("Standard Render Pipeline (legacy)");
	} IMGUI.End();

	IMGUI.EndFrame();
}

#pragma endregion