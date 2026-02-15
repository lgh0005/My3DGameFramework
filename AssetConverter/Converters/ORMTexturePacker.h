#pragma once
#include "AssetTypes/AssetFormat.h"

class ORMTexturePacker
{
	DECLARE_SINGLE(ORMTexturePacker)

public:
    bool Convert
    (
        const std::string& aoPath,
        const std::string& roughPath,
        const std::string& metalPath,
        const std::string& outPngPath,
        bool invertRoughness = false,
        bool flipY = true
    );

    bool Pack
    (
        const std::string& aoPath,
        const std::string& roughPath,
        const std::string& metalPath,
        AssetFmt::RawImage& outImage,
        bool invertRoughness = false,
        bool flipY = true
    );

private:

    bool LoadImageToRaw
    (
        const std::string& filepath, 
        AssetFmt::RawImage& outImage,
        bool flipY = true
    );

    void FlipImageVertically(uint8* data, int32 w, int32 h, int32 ch);

    uint8 GetPixelChannel
    (
        const AssetFmt::RawImage& img, 
        int32 px, int32 py
    );
};