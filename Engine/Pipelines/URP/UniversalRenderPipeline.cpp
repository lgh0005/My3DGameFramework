#include "EnginePch.h"
#include "UniversalRenderPipeline.h"

#include "Pipelines/Common/CullingPass.h"
#include "Pipelines/URP/RenderPasses/UniversalSkyboxPass.h"
#include "Pipelines/URP/UniversalGlobalUniforms.h"
#include "Pipelines/URP/UniversalRenderContext.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Core/RenderPass.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"

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

	// Skybox 패스 생성
	m_skyboxPass = UniversalSkyboxPass::Create();
	if (!m_skyboxPass) return false;

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
	context.SetSkyboxTexture(scene->GetSkyboxTexture());

	// [패스 0] 컬링 패스 : 절두체 범위 안에 있는 대상만 추리기
	m_cullingPass->Render(&context);

	// 1. ubo 갱신
	m_globalUniforms->PreRender(&context);

	// [패스 5] 포워드 셰이딩
	auto& cnt = scene->GetCustomRenderPasses();
	for (const auto& [name, pass] : scene->GetCustomRenderPasses())
		pass->Render(scene, camera);

	// [패스 6] 스카이박스 패스
	m_skyboxPass->Render(&context);

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