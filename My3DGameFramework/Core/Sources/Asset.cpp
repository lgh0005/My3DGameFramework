#include "CorePch.h"
#include "Asset.h"
#include "Resource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Asset::Asset(const String& path) : m_path(path) { }
	Asset::~Asset() = default;

	/*==============================//
	//       Asset Type             //
	//==============================*/
	int16 Asset::s_typeIndex = -1;
	const MGFType* Asset::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void Asset::AddResource(const ResourcePtr& resource)
	{
		m_resources.push_back(resource);
	}
}