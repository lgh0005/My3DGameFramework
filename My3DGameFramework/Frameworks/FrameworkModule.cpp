#include "FrameworkPch.h"
#include "FrameworkModule.h"

#pragma region MANAGERS
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/RenderManager.h"
#include "Managers/EntityManager.h"
#include "Managers/TypeManager.h"
#pragma endregion

#pragma region TYPES
#include "Identities/MGFTypeTree.h"
#pragma endregion

#pragma region RENDER_PASS
#include "Rendering/RenderPass.h"
#pragma endregion

#pragma region RENDER_PIPELINE
#include "Rendering/RenderPipeline.h"
#pragma endregion

#pragma region COMPONENT
#include "Registries/ComponentRegistry.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#pragma endregion

#pragma region RESOURCE
#include "Resources/Material.h"
#pragma endregion

namespace MGF3D
{
	void FrameworkModule::OnRegisterTypes()
	{
		// 1. Resources 타입 베이킹
		MGFTypeTree* resourceTree = MGF_TYPE.GetTree("Resource");
		Material::s_typeIndex = resourceTree->Register("Material", "Resource");

		// 1. RenderPass 타입 트리 생성
		MGFTypeTree& renderPassTree = MGF_TYPE.CreateTree("RenderPass");
		RenderPass::s_typeIndex = renderPassTree.Register("RenderPass", "");

		// 1. RenderPieline 타입 트리 생성
		MGFTypeTree& renderPipelineTree = MGF_TYPE.CreateTree("RenderPipeline");
		RenderPipeline::s_typeIndex = renderPipelineTree.Register("RenderPipeline", "");

		// 2. Components 타입 베이킹
		MGFTypeTree* componentTree = MGF_TYPE.GetTree("Component");
		MeshRenderer::s_typeIndex = componentTree->Register("MeshRenderer", "");

		// 3. Component 레지스트리 주입
		auto meshRendererReg = MakeUnique<ComponentRegistry<MeshRenderer>>();
		MGF_ENTITY.AddComponentRegistry(MeshRenderer::s_typeIndex, std::move(meshRendererReg));
	}

	bool FrameworkModule::OnInit()
	{
		// 1. 렌더 매니저 초기화
		if (!MGF_RENDER.Init()) return false;

		return true;
	}

	bool FrameworkModule::OnShutdown()
	{
		// 0. 리소스 매니저 종료
		MGF_RESOURCE.Shutdown();

		// 1. 에셋 매니저 종료
		MGF_ASSET.Shutdown();

		// 3. 렌더 매니저 종료
		MGF_RENDER.Shutdown();

		return true;
	}
}