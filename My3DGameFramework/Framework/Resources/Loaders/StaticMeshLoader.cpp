#include "ResourcePch.h"
#include "StaticMeshLoader.h"
#include "Descriptors/StaticMeshDescriptor.h"
#include "Meshes/StaticMesh.h"

namespace MGF3D
{
	StaticMeshLoader::StaticMeshLoader() = default;
	StaticMeshLoader::~StaticMeshLoader() = default;

	bool StaticMeshLoader::Load
	(
		ResourcePtr& outResource,
		IResourceDescriptorUPtr desc
	)
	{
		// 1. 안전한 캐스팅 (RTTI 대신 typeID 체크)
		if (desc->typeID != "StaticMesh"_sh) return false;

		// 2. Descriptor를 구체적인 타입으로 취급 (정적 캐스팅)
		auto* meshDesc = static_cast<Ptr<StaticMeshDescriptor>>(desc.get());

		// 3. StaticMesh의 정적 팩토리 메서드 호출
		auto mesh = StaticMesh::Create(std::move(*meshDesc));
		if (!mesh) return false;

		// 4. 결과물 전달
		outResource = mesh;
		return true;
	}
}