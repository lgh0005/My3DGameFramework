#include "pch.h"
#include "AssetFormat.h"

bool AssetFmt::RawImage::IsValid() const
{
	return (!pixels.empty() && width > 0 && height > 0);
}

std::string AssetFmt::RawMaterial::GetTexturePath(RawTextureType type) const
{
	for (const auto& tex : textures)
	{
		if (tex.type == type) return tex.fileName;
	}
	return "";
}

AssetFmt::RawSkinnedVertex::RawSkinnedVertex()
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		boneIDs[i] = -1;
		weights[i] = 0.0f;
	}
}