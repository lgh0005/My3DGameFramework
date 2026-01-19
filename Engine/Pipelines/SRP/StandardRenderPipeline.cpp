#include "EnginePch.h"
#include "StandardRenderPipeline.h"

#include "Pipelines/Common/CullingPass.h"
#include "Pipelines/Common/ShadowPass.h"
#include "Pipelines/Common/SkyboxPass.h"
#include "Pipelines/Common/JoltDebugGizmoPass.h"
#include "Pipelines/Common/OutlinePass.h"
#include "Pipelines/Common/SSAOPass.h"
#include "Pipelines/Common/MotionBlurPass.h"
#include "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"
#include "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"
#include "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"
#include "Pipelines/SRP/StandardGlobalUniforms.h"

#include "Scene/Scene.h"
#include "Scene/ComponentRegistry.h"
#include "Object/GameObject.h"
#include "Resources/Program.h"
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
	m_outlinePass = OutlinePass::Create();
	if (!m_outlinePass) return false;

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

	// 포스트-프로세스 패스 생성
	m_postProcessPass = StandardPostProcessPass::Create();
	m_postProcessPass->GetFramebuffer()->CreateAndAttachDepth();
	if (!m_postProcessPass) return false;

	// G-buffer 패스 생성
	m_geometryPass = StandardGeometryPass::Create();
	m_geometryPass->GetGBuffer()->CreateAndAttachDepth();
	if (!m_geometryPass) return false;

	// Light 패스 생성
	m_deferredLightPass = StandardDeferredLightingPass::Create();
	if (!m_deferredLightPass) return false;

	// 모션 블러 패스 생성
	m_motionBlurPass = MotionBlurPass::Create();
	if (!m_motionBlurPass) return false;

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

	// [패스 6] 스카이박스 패스 (공유된 깊이를 기반으로 깊이 테스트 수행하며 덧그림)
	m_skyboxPass->Render(&context);

	auto tmp = m_postProcessPass->GetFramebuffer();

	// [패스 7] 모션 블러 & 아웃라인 (2D 후처리 효과들)
	m_motionBlurPass->Render(&context);
	m_outlinePass->Render(&context);

	// [패스 8] 후처리 패스 (최종 합성 및 화면 출력)
	m_postProcessPass->Render(&context);

	// [패스 9] 디버그 패스 (ImGUI 컨텍스트와 충돌 기즈모 출력)
	// m_debugGizmoPass->Render(&context);

	RenderIMGUIContext();
}

void StandardRenderPipeline::OnResize(int32 width, int32 height)
{
	// 1. 모든 패스의 FBO 내부 텍스처 갱신
	m_geometryPass->Resize(width, height);
	m_postProcessPass->Resize(width, height);
	m_ssaoPass->Resize(width, height);
	m_outlinePass->Resize(width, height);

	// 2. 깊이 버퍼 재연결
	auto mainDepth = m_geometryPass->GetGBuffer()->GetDepthAttachment();
	m_postProcessPass->GetFramebuffer()->AttachDepthTexture(mainDepth);
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