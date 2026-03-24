#include "CorePch.h"
#include "Asset.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	Asset::Asset(const IAssetDescriptor& desc)
		: m_type(desc.type), m_name(desc.name), m_path(desc.path) { }
	Asset::~Asset() = default;
}