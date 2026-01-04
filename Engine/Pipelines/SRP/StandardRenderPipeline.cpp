#include "EnginePch.h"
#include "StandardRenderPipeline.h"

#include "Pipelines/Common/CullingPass.h"
#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/Common/SkyboxPass.h"
#include "Pipelines/SRP/RenderPasses/StandardOutlinePass.h"
#include "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
#include "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"
#include "Pipelines/SRP/RenderPasses/StandardSSAOPass.h"
#include "Pipelines/SRP/StandardGlobalUniforms.h"
#include "Pipelines/SRP/StandardRenderContext.h"

#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/SceneRegistry.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h" 
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"
#include "Framebuffers/PostProcessFramebuffer.h"
#include "Framebuffers/GBufferFramebuffer.h"

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

	// 아웃라인 패스 생성
	m_outlinePass = StandardOutlinePass::Create();
	if (!m_outlinePass) return false;

	// 셰도우 패스 생성
	m_shadowPass = ShadowPass::Create();
	if (!m_shadowPass) return false;

	// 스카이박스 패스 생성
	m_skyboxPass = SkyboxPass::Create();
	if (!m_skyboxPass) return false;

	// SSAO 패스 생성
	m_ssaoPass = StandardSSAOPass::Create();
	if (!m_ssaoPass) return false;

	// 포스트-프로세스 패스 생성
	m_postProcessPass = StandardPostProcessPass::Create();
	if (!m_postProcessPass) return false;

	// G-buffer 패스 생성
	m_geometryPass = StandardGeometryPass::Create();
	if (!m_geometryPass) return false;

	// Light 패스 생성
	m_deferredLightPass = StandardDeferredLightingPass::Create();
	if (!m_deferredLightPass) return false;

	return true;
}

void StandardRenderPipeline::BlitCopyDepth
(
	GBufferFramebuffer* src, PostProcessFramebuffer* dst,
	int32 width,		 int32 height
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

void StandardRenderPipeline::Render(Scene* scene)
{
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	// TODO : 이후에는 여러 카메라를 지원할 것이라면 카메라 개수 만큼
	// 반복해야 할 수도 있음.
	auto* registry = scene->GetRegistry();
	auto* camera = registry->GetCamera(0);
	if (!camera) return;

	// 0. 스택 영역에 StandardRenderContext 생성
	StandardRenderContext context;
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
	for (const auto& [name, pass] : registry->GetCustomRenderPasses())
		pass->Render(scene, camera);

	// ====================================================
	// [패스 5.5] World Space UI 패스 (3D 공간 UI)
	// ====================================================
	// 위치: 후처리(Pass 8) 전이어야 함! 그래야 Bloom도 먹고 톤맵핑도 먹음.
	// 설정: Depth Test 켜야 함 (벽 뒤에 숨어야 하니까)
	// m_uiPass->Render(&context, UIRenderMode::WorldSpace);

	// [패스 6] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	m_skyboxPass->Render(&context);

	// [패스 7] 아웃라인 패스
	m_outlinePass->Render(&context);

	// [패스 8] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	m_postProcessPass->Render(&context);

	// ====================================================
	// [패스 9] UI 패스 : 최종 화면 위에 UI 덧그리기 (Overlay)
	// ====================================================
	// 이 시점에서는 Depth Test를 끄고(Screen UI), Alpha Blending을 켜야 함

	/*=========================//
	//   imgui debug context   //
	//=========================*/
	RenderIMGUIContext();
}

void StandardRenderPipeline::OnResize(int32 width, int32 height)
{
	m_geometryPass->Resize(width, height);
	m_postProcessPass->Resize(width, height);
	m_ssaoPass->Resize(width, height);
	m_outlinePass->Resize(width, height);
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