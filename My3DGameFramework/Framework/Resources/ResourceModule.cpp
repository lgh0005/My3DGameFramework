#include "ResourcePch.h"
#include "ResourceModule.h"
#include "Managers/ResourceManager.h"

// 리소스 타입 헤더 포함
#include "Resource/Resource.h"
#include "Meshes/Mesh.h"
#include "Meshes/ScreenMesh.h"
#include "Loaders/ScreenMeshLoader.h"
#include "Meshes/StaticMesh.h"
#include "Loaders/StaticMeshLoader.h"
#include "Meshes/SkinnedMesh.h"
#include "Loaders/SkinnedMeshLoader.h"

namespace MGF3D
{
	ResourceModule::ResourceModule() = default;
	ResourceModule::~ResourceModule() = default;

	bool ResourceModule::OnModuleInit()
	{
		MGF_LOG_INFO("ResourceInitializer: Baking Resource Types...");

		// 1. 최상위 부모 타입 초기화
		Resource::InitializeType();

		// 2. 중간 부모 타입 초기화
		Mesh::InitializeType();

		// 3. 자식 타입들 초기화
		ScreenMesh::InitializeType();
		MGF_RESOURCE.RegisterLoader
		(
			ScreenMesh::s_type.Get(),
			MakeUnique<ScreenMeshLoader>()
		);

		StaticMesh::InitializeType();
		MGF_RESOURCE.RegisterLoader
		(
			StaticMesh::s_type.Get(),
			MakeUnique<StaticMeshLoader>()
		);

		SkinnedMesh::InitializeType();
		MGF_RESOURCE.RegisterLoader(
			SkinnedMesh::s_type.Get(),
			MakeUnique<SkinnedMeshLoader>()
		);

		return true;
	}

	void ResourceModule::OnShutDown()
	{
		SkinnedMesh::s_type.Reset();
		StaticMesh::s_type.Reset();
		ScreenMesh::s_type.Reset();

		Mesh::s_type.Reset();
		Resource::s_type.Reset();

		MGF_LOG_INFO("ResourceInitializer: Resource Types Shut Down.");
	}
}