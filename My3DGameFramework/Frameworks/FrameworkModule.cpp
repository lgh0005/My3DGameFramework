#include "FrameworkPch.h"
#include "FrameworkModule.h"

#pragma region MANAGERS
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/RenderManager.h"
#include "Managers/ScriptManager.h"
#include "Managers/AnimationManager.h"
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
#include "Components/MeshRenderers/SkinnedMeshRenderer.h"
#include "Components/Script/Script.h"
#include "Components/Lights/Light.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/SpotLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/SkyLight.h"
#include "Components/Animation/Animator.h"
#pragma endregion

#pragma region MECHANICS
#include "Mechanics/Animation/AnimController.h"
#include "Mechanics/Lights/SkyCube.h"
#pragma endregion

#pragma region RESOURCE
#include "Resources/Material.h"
#include "Resources/EnvironmentMap.h"
#pragma endregion

#pragma region ASSET
#include "Assets/Model.h"
#include "Assets/Animation.h"
#pragma endregion

namespace MGF3D
{
	void FrameworkModule::OnRegisterTypes()
	{
		// 0. Asset 타입
		MGFTypeTree* assetTree = MGF_TYPE.GetTree("Asset");
		Model::s_typeIndex = assetTree->Register("Model", "Asset");
		Animation::s_typeIndex = assetTree->Register("Animation", "Asset");

		// 1. Resources 타입 베이킹
		MGFTypeTree* resourceTree = MGF_TYPE.GetTree("Resource");
		Material::s_typeIndex = resourceTree->Register("Material", "Resource");
		EnvironmentMap::s_typeIndex = resourceTree->Register("Material", "EnvironmentMap");

		// 1. RenderPass 타입 트리 생성
		MGFTypeTree& renderPassTree = MGF_TYPE.CreateTree("RenderPass");
		RenderPass::s_typeIndex = renderPassTree.Register("RenderPass", "");

		// 1. RenderPieline 타입 트리 생성
		MGFTypeTree& renderPipelineTree = MGF_TYPE.CreateTree("RenderPipeline");
		RenderPipeline::s_typeIndex = renderPipelineTree.Register("RenderPipeline", "");

		// 2. Components 타입 베이킹
		MGFTypeTree* componentTree = MGF_TYPE.GetTree("Component");
		MeshRenderer::s_typeIndex = componentTree->Register("MeshRenderer", "");
		SkinnedMeshRenderer::s_typeIndex = componentTree->Register("SkinnedMeshRenderer", "MeshRenderer");
		Script::s_typeIndex = componentTree->Register("Script", "");
		Light::s_typeIndex = componentTree->Register("Light", "");
		DirectionalLight::s_typeIndex = componentTree->Register("DirectionalLight", "Light");
		SpotLight::s_typeIndex = componentTree->Register("SpotLight", "Light");
		PointLight::s_typeIndex = componentTree->Register("PointLight", "Light");
		SkyLight::s_typeIndex = componentTree->Register("SkyLight", "");
		Animator::s_typeIndex = componentTree->Register("Animator", "");

		// 3. 머캐닉 타입
		MGFTypeTree* mechanicsTree = MGF_TYPE.GetTree("Mechanics");
		SkyCube::s_typeIndex = mechanicsTree->Register("SkyCube", "");
		AnimController::s_typeIndex = mechanicsTree->Register("AnimController", "");

		// 3. Component 레지스트리 주입
		MGF_ENTITY.AddComponentRegistry(MeshRenderer::s_typeIndex, MakeUnique<ComponentRegistry<MeshRenderer>>());
		MGF_ENTITY.AddComponentRegistry(SkinnedMeshRenderer::s_typeIndex, MakeUnique<ComponentRegistry<SkinnedMeshRenderer>>());
		MGF_ENTITY.AddComponentRegistry(DirectionalLight::s_typeIndex, MakeUnique<ComponentRegistry<DirectionalLight>>());
		MGF_ENTITY.AddComponentRegistry(PointLight::s_typeIndex, MakeUnique<ComponentRegistry<PointLight>>());
		MGF_ENTITY.AddComponentRegistry(SpotLight::s_typeIndex, MakeUnique<ComponentRegistry<SpotLight>>());
		MGF_ENTITY.AddComponentRegistry(Animator::s_typeIndex, MakeUnique<ComponentRegistry<Animator>>());
		MGF_ENTITY.AddComponentRegistry(SkyLight::s_typeIndex, MakeUnique<ComponentRegistry<SkyLight>>());
	}

	bool FrameworkModule::OnInit()
	{
		// 1. 렌더 매니저 초기화
		if (!MGF_RENDER.Init()) return false;

		// 2. 스크립트 매니저 초기화
		if (!MGF_SCRIPT.Init()) return false;

		// 3. 애니메이션 매니저 초기화
		if (!MGF_ANIM.Init()) return false;

		return true;
	}

	bool FrameworkModule::OnShutdown()
	{
		// 0. 리소스 매니저 종료
		MGF_RESOURCE.Shutdown();

		// 1. 에셋 매니저 종료
		MGF_ASSET.Shutdown();

		// 2. 애니메이션 매니저 종료
		MGF_ANIM.Shutdown();

		// 2. 스크립트 매니저 종료
		MGF_SCRIPT.Shutdown();

		// 3. 렌더 매니저 종료
		MGF_RENDER.Shutdown();

		return true;
	}
}