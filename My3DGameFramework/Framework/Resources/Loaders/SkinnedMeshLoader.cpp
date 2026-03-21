#include "ResourcePch.h"
#include "SkinnedMeshLoader.h"
#include "Descriptors/SkinnedMeshDescriptor.h"
#include "Meshes/SkinnedMesh.h"

namespace MGF3D
{
	SkinnedMeshLoader::SkinnedMeshLoader() = default;
	SkinnedMeshLoader::~SkinnedMeshLoader() = default;

	bool SkinnedMeshLoader::Load
	(
		ResourcePtr& outResource, 
		IResourceDescriptorUPtr desc
	)
	{
		// 1. 안전한 캐스팅 (RTTI 대신 typeID 체크)
		if (desc->typeID != "StaticMesh"_sh) return false;

		// 2. Descriptor를 구체적인 타입으로 취급 (정적 캐스팅)
		auto* meshDesc = static_cast<Ptr<SkinnedMeshDescriptor>>(desc.get());

		// 3. SkinnedMesh의 정적 팩토리 메서드 호출
		auto mesh = SkinnedMesh::Create(std::move(*meshDesc));
		if (!mesh) return false;

		// 4. 결과물 전달
		outResource = mesh;
		return true;
	}


}