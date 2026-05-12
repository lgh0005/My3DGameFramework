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

#pragma once RENDER_PIPELINE
#include "Pipelines/MGFRenderPipeline.h"
#include "Pipelines/RenderPasses/MGFShadowPass.h"
#include "Pipelines/RenderPasses/MGFGeometryPass.h"
#include "Pipelines/RenderPasses/MGFSSAOPass.h"
#include "Pipelines/RenderPasses/MGFDeferredLightingPass.h"
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

		return true;
	}

	bool RuntimeModule::OnShutdown()
	{
		return true;
	}
}