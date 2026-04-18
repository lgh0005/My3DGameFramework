#include "FrameworkPch.h"
#include "FrameworkModule.h"

#pragma region MANAGERS
#include "Managers/PathManager.h"
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
// #include "Managers/RenderManager.h"
#pragma endregion

#pragma region TYPES
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"
#pragma endregion

#pragma region COMPONENT
#include "Entities/Component.h"
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

		// 2. Components 타입 베이킹
		MGFTypeTree* componentTree = MGF_TYPE.GetTree("Component");
		MeshRenderer::s_typeIndex = componentTree->Register("MeshRenderer", "Component");

		// 3. Component 레지스트리 주입
	}

	bool FrameworkModule::OnInit()
	{		

		return true;
	}

	bool FrameworkModule::OnShutdown()
	{
		// 0. 리소스 매니저 종료

		// 1. 에셋 매니저 종료
		MGF_ASSET.Shutdown();

		// 2. 경로 매니저 종료
		
		// 3. 렌더 매니저 종료


		return true;
	}
}