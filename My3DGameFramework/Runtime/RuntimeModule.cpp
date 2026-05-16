#include "RuntimePch.h"
#include "RuntimeModule.h"
#include "Managers/TypeManager.h"
#include "Scene/Scene.h"

#pragma region MANAGERS
#include "Managers/TypeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/RenderManager.h"
#include "Managers/GUIManager.h"
#pragma endregion

#pragma region TYPES
#include "Identities/MGFTypeTree.h"
#pragma endregion

#pragma region RENDER_PIPELINE
#include "Pipelines/MGFRenderPipeline.h"
#include "Pipelines/RenderPasses/MGFShadowPass.h"
#include "Pipelines/RenderPasses/MGFGeometryPass.h"
#include "Pipelines/RenderPasses/MGFSSAOPass.h"
#include "Pipelines/RenderPasses/MGFDeferredLightingPass.h"
#pragma endregion

#pragma region GUI_WINDOWS
#include "Editor/Windows/GUISceneViewport.h"
#include "Editor/Windows/GUIInspectorViewport.h"
#include "Editor/Windows/GUIConsoleViewport.h"
#include "Editor/Windows/GUIHierarchyViewport.h"
#include "Editor/Windows/GUIProjectViewport.h"
#pragma endregion

namespace MGF3D
{
	void RuntimeModule::OnRegisterTypes()
	{
		// 1. 씬 타입 트리 생성
		MGFTypeTree& sceneTree = MGF_TYPE.CreateTree("Scene");
		Scene::s_typeIndex = sceneTree.Register("Scene", "");

		// 2. 테스트 렌더링 파이프라인 타입 트리 생성
		MGFTypeTree* renderPipelineTree = MGF_TYPE.GetTree("RenderPipeline");
		MGFRenderPipeline::s_typeIndex = renderPipelineTree->Register("MGFRenderPipeline", "RenderPipeline");

		// 3. 렌더 패스 파이프라인 타입트리
		MGFTypeTree* renderPassTree = MGF_TYPE.GetTree("RenderPass");
		MGFGeometryPass::s_typeIndex = renderPassTree->Register("MGFGeometryPass", "RenderPass");
		MGFSSAOPass::s_typeIndex = renderPassTree->Register("MGFSSAOPass", "RenderPass");
		MGFDeferredLightingPass::s_typeIndex = renderPassTree->Register("MGFDeferredLightingPass", "RenderPass");
	}

	bool RuntimeModule::OnInit()
	{
		// 1. 렌더 매니저에 테스트용 파이프라인 등록
		MGF_RENDER.RegisterRenderPipeline<MGFRenderPipeline>("MGFRenderPipeline");

		// 2. 현재 활성 파이프라인으로 설정
		MGF_RENDER.SetRenderPipeline("MGFRenderPipeline");

		// 3. 에디터 초기화
		MGF_EDITOR.Init(MGF_WINDOW.GetNativeHandle());

		// 4. 에디터 윈도우 주입
		MGF_EDITOR.AddGUIWindow(SceneViewport::Create("Scene View"));
		MGF_EDITOR.AddGUIWindow(InspectorViewport::Create("Inspector View"));
		MGF_EDITOR.AddGUIWindow(ConsoleViewport::Create("Console View"));
		MGF_EDITOR.AddGUIWindow(HierarchyViewport::Create("Hierarchy View"));
		MGF_EDITOR.AddGUIWindow(ProjectViewport::Create("Project View"));

		return true;
	}

	bool RuntimeModule::OnShutdown()
	{
		// 1. GUI 매니저 종료
		MGF_EDITOR.Shutdown();

		return true;
	}
}