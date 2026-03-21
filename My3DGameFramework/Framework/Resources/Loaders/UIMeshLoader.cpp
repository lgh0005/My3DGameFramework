#include "ResourcePch.h"
#include "UIMeshLoader.h"
#include "Descriptors/UIMeshDescriptor.h"
#include "Meshes/UIMesh.h"

namespace MGF3D
{
	UIMeshLoader::UIMeshLoader() = default;
	UIMeshLoader::~UIMeshLoader() = default;

	bool UIMeshLoader::Load
	(
		ResourcePtr& outResource, 
		IResourceDescriptorUPtr desc
	)
	{
		// 1. 안전한 캐스팅 (RTTI 대신 typeID 체크)
		if (desc->typeID != "UIMesh"_sh) return false;

		// 2. Descriptor를 구체적인 타입으로 취급 (정적 캐스팅)
		auto* meshDesc = static_cast<Ptr<UIMeshDescriptor>>(desc.get());

		// 3. UIMesh의 정적 팩토리 메서드 호출
		auto mesh = UIMesh::Create(std::move(*meshDesc));
		if (!mesh) return false;

		// 4. 결과물 전달
		outResource = mesh;
		return true;
	}
}