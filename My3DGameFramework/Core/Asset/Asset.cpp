#include "CorePch.h"
#include "Asset.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	Asset::Asset(const IAssetDescriptor& desc) : Super(desc.type, desc.name) { }
	Asset::~Asset() = default;
}