#pragma once
#include "Pointer/PoolAlloc.h"
#include "Asset/Asset.h"
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Asset)

    MGF_STRUCT_PTR(IAssetImporter)
    struct IAssetImporter : public PoolAlloc
    {
        IAssetImporter();
        virtual ~IAssetImporter();

        virtual bool Load
        (
            AssetPtr& outAsset, 
            IAssetDescriptorUPtr desc
        ) = 0;
    };
}