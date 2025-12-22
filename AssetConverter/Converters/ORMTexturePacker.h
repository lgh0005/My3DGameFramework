#pragma once
#include "AssetTypes/AssetFormat.h"

class ORMTexturePacker
{
	DECLARE_SINGLE(ORMTexturePacker)

public:
    bool Convert(const std::string& aoPath,
                 const std::string& roughPath,
                 const std::string& metalPath,
                 const std::string& outPngPath,
                 bool invertRoughness = false);

private:

    bool LoadImageToRaw
    (
        const std::string& filepath, 
        AssetFmt::RawImage& outImage
    );

    uint8 GetPixelChannel
    (
        const AssetFmt::RawImage& img, 
        int32 px, int32 py
    );
};