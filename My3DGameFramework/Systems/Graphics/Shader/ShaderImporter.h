#pragma once
#include "Asset/IAssetImporter.h"

namespace MGF3D
{
	struct ShaderImporter : public IAssetImporter
	{
		ShaderImporter();
		virtual ~ShaderImporter() override;

		virtual bool Load
		(
			AssetPtr& outAsset,
			IAssetDescriptorUPtr desc
		) override;
	};
}