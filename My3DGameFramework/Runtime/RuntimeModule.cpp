#include "RuntimePch.h"
#include "RuntimeModule.h"
#include "Managers/TypeManager.h"
#include "Scene/Scene.h"

#pragma region MANAGERS
#include "Managers/TypeManager.h"
#include "Managers/RenderManager.h"
#pragma endregion

#pragma region TYPES
#include "Identities/MGFTypeTree.h"
#pragma endregion

#pragma once TEST_RENDERING
#include "Pipelines/_Test/TestPipeline.h"
#include "Pipelines/_Test/RenderPasses/TestRenderPass.h"
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
		TestPipeline::s_typeIndex = renderPipelineTree->Register("TestPipeline", "RenderPipeline");
	}

	bool RuntimeModule::OnInit()
	{
		// 1. 렌더 매니저에 테스트용 파이프라인 등록
		MGF_RENDER.RegisterRenderPipeline<TestPipeline>("TestPipeline");

		// 2. 현재 활성 파이프라인으로 설정
		MGF_RENDER.SetRenderPipeline("TestPipeline");

		return true;
	}

	bool RuntimeModule::OnShutdown()
	{
		return true;
	}
}