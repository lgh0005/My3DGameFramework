#include "EnginePch.h"
#include "UniversalRenderPipeline.h"

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
	// TODO : PBR 렌더링을 위한 URP 렌더패스들을 Create

	return true;
}

void UniversalRenderPipeline::Render(Scene* scene)
{
	// TODO : PBR 렌더링을 위한 URP 렌더패스들 지나면서 Context 기반 렌더링 수행
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// TEMP : 뷰포트 설정 (이게 없으면 윈도우 크기가 0x0이거나 이상하게 잡혀서 안 보임)
	// 매 프레임 창 크기에 맞춰 갱신해주는 것이 안전합니다.
	// 지금은 고정 Context 렌더 패스를 추가하지 않아서 뷰포트 설정을 일단 여기서 해주긴 해야함
	{
		int width, height;
		glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
		glViewport(0, 0, width, height);

		// 2. 화면 클리어 (이게 없으면 이전 프레임 잔상 위에 그리거나 검은 화면)
		//    배경색을 약간 밝게(남색) 해서 검은색 구체라도 보이게 합니다.
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 3. 깊이 테스트 켜기 (3D 렌더링 필수)
		//    ImGui가 이걸 꺼버리는 경우가 있어서, 3D 그리기 전에 반드시 켜줘야 합니다.
		glEnable(GL_DEPTH_TEST);
	}

	// [패스 5] 포워드 셰이딩
	for (const auto& [name, pass] : scene->GetCustomRenderPasses())
		pass->Render(scene, camera);

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