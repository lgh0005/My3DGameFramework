#include "pch.h"
#include "RawMaterial.h"

std::string RawMaterial::GetTexturePath(RawTextureType type) const
{
	for (const auto& tex : textures)
	{
		if (tex.type == type) return tex.fileName;
	}
	return "";
}