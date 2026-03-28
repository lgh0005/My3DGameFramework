#include "CorePch.h"
#include "Asset.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	Asset::Asset(const IAssetDescriptor& desc)
		: Super(desc.type, desc.name), m_path(desc.path) { }
	Asset::~Asset() = default;

	/*==================================//
	//         Type System              //
	//==================================*/
	UniquePtr<MGFType> Asset::s_type = nullptr;

	void Asset::InitializeType()
	{
		s_type = MakeUnique<MGFType>();

		s_type->name = MGFName("Asset");
		s_type->id = StringHash("Asset");

		s_type->parent = nullptr;
		s_type->depth = 0;
		s_type->chain[0] = s_type->id;
	}

	const MGFType* Asset::GetType() const
	{
		return s_type.Get();
	}
}