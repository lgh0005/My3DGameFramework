#include "ResourcePch.h"
#include "ScreenMeshLoader.h"
#include "Descriptors/ScreenMeshDescriptor.h"
#include "Meshes/ScreenMesh.h"

namespace MGF3D
{
	ScreenMeshLoader::ScreenMeshLoader() = default;
	ScreenMeshLoader::~ScreenMeshLoader() = default;

	bool ScreenMeshLoader::Load
	(
		ResourcePtr& outResource, 
		IResourceDescriptorUPtr desc
	)
	{
		// 1. 안전한 캐스팅 (RTTI 대신 typeID 체크)
		if (desc->typeID != "ScreenMesh"_sh) return false;

		// 2. Descriptor를 구체적인 타입으로 취급 (정적 캐스팅)
		auto* meshDesc = static_cast<Ptr<ScreenMeshDescriptor>>(desc.get());

		// 3. ScreenMesh의 정적 팩토리 메서드 호출
		auto mesh = ScreenMesh::Create(std::move(*meshDesc));
		if (!mesh) return false;

		// 4. 결과물 전달
		outResource = mesh;
		return true;
	}
}