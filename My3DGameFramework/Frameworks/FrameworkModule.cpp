#include "FrameworkPch.h"
#include "FrameworkModule.h"
#include "Managers/TypeManager.h"
#include "Managers/EntityManager.h"
#include "Identities/MGFTypeTree.h"

#pragma region COMPONENT
#include "Entities/Component.h"
#include "Registries/ComponentRegistry.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#include "Components/MeshRenderers/StaticMeshRenderer.h"
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
		StaticMeshRenderer::s_typeIndex = componentTree->Register("StaticMeshRenderer", "MeshRenderer");

		// 3. Component 레지스트리 주입
	}

	bool FrameworkModule::OnInit()
	{
		return true;
	}

	bool FrameworkModule::OnShutdown()
	{
		return true;
	}
}