#include "EnginePch.h"
#include "UniversalRenderPipeline.h"

#include "Pipelines/Common/CullingPass.h"
#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/Common/SkyboxPass.h"
#include "Pipelines/URP/RenderPasses/UniversalGeometryPass.h"
#include "Pipelines/URP/RenderPasses/UniversalSSAOPass.h"
#include "Pipelines/URP/RenderPasses/UniversalPostProcessPass.h"
#include "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.h"
#include "Pipelines/URP/UniversalGlobalUniforms.h"
#include "Pipelines/URP/UniversalRenderContext.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Core/RenderPass.h"
#include "Graphics/Program.h"
#include "Graphics/ShadowMap.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h" 
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"
#include "Framebuffers/PostProcessFramebuffer.h"
#include "Framebuffers/GBufferFramebuffer.h"

UniversalRenderPipelineUPtr UniversalRenderPipeline::Create()
{
	auto renderPipeline = UniversalRenderPipelineUPtr(new UniversalRenderPipeline());
	if (!renderPipeline->Init()) return nullptr;
	return std::move(renderPipeline);
}

bool UniversalRenderPipeline::Init()
{
	// UBO 생성
	m_globalUniforms = UniversalGlobalUniforms::Create();
	if (!m_globalUniforms) return false;

	// 컬링 패스 생성
	m_cullingPass = CullingPass::Create();
	if (!m_cullingPass) return false;

	// 셰도우 패스 생성
	m_shadowPass = ShadowPass::Create();
	if (!m_shadowPass) return false;

	// Skybox 패스 생성
	m_skyboxPass = SkyboxPass::Create();
	if (!m_skyboxPass) return false;

	// SSAO 패스 생성
	m_ssaoPass = UniversalSSAOPass::Create();
	if (!m_ssaoPass) return false;

	// 포스트-프로세싱 패스 생성
	m_postProcessPass = UniversalPostProcessPass::Create();
	if (!m_postProcessPass) return false;

	// G-buffer 패스 생성
	m_geometryPass = UniversalGeometryPass::Create();
	if (!m_geometryPass) return false;

	// Light 패스 생성
	m_deferredLightPass = UniversalDeferredLightingPass::Create();
	if (!m_deferredLightPass) return false;

	return true;
}

void UniversalRenderPipeline::Render(Scene* scene)
{
	// TEMP : 첫 뷰포트 설정.
	int width, height;
	glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// TODO : PBR 렌더링을 위한 URP 렌더패스들 지나면서 Context 기반 렌더링 수행
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// 0. 스택 영역에 StandardRenderContext 생성
	UniversalRenderContext context;
	context.Reset(scene, camera);

	// [패스 0] 컬링 패스 : 절두체 범위 안에 있는 대상만 추리기
	m_cullingPass->Render(&context);

	// 1. ubo 갱신
	m_globalUniforms->PreRender(&context);

	/*================================//
	//   main scene rendergin logic   //
	//================================*/
	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	m_shadowPass->Render(&context);

	// [패스 2] 디퍼드 셰이딩 : G-buffer 채우기
	m_geometryPass->Render(&context);

	// [패스 3] SSAO
	m_ssaoPass->Render(&context);

	// [패스 4] Deferred Lighting 패스: G-buffer를 기반으로 라이팅 연산 시작
	// 포스트 프로세싱을 위해 포스트 프로세스 프레임 버퍼게 그림을 그림
	context.SetTargetFramebuffer(m_postProcessPass->GetFramebuffer());
	m_deferredLightPass->Render(&context);

	// 포스트 프로세스 프레임 버퍼에 깊이 복사
	auto gBuffer = m_geometryPass->GetGBuffer();
	auto postFBO = m_postProcessPass->GetFramebuffer();
	BlitCopyDepth(gBuffer, postFBO, gBuffer->GetWidth(), gBuffer->GetHeight());

	// [패스 5] 포워드 셰이딩
	for (const auto& [name, pass] : scene->GetCustomRenderPasses())
		pass->Render(scene, camera);

	// [패스 6] 스카이박스 패스
	m_skyboxPass->Render(&context);

	// [패스 7] 포스트 프로세싱 패스
	m_postProcessPass->Render(&context);

	/*=========================//
	//   imgui debug context   //
	//=========================*/
	RenderIMGUIContext();
}

void UniversalRenderPipeline::OnResize(int32 width, int32 height)
{
	// TODO
}

/*======================//
//  debug imgui method  //
//======================*/
#pragma region IMGUI_DEBUG_METHOD

void UniversalRenderPipeline::RenderIMGUIContext()
{
	IMGUI.BeginFrame();

	if (IMGUI.Begin("Test Window"))
	{
		ImGui::Text("Default Render Pipeline #2");
		ImGui::Text("Universal Render Pipeline (PBR)");
	} IMGUI.End();

	IMGUI.EndFrame();
}

#pragma endregion

void UniversalRenderPipeline::BlitCopyDepth
(
	GBufferFramebuffer* src, PostProcessFramebuffer* dst,
	int32 width, int32 height
)
{
	if (!src || !dst) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->Get());

	glBlitFramebuffer
	(
		0, 0, width, height,
		0, 0, width, height,
		GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);
}
