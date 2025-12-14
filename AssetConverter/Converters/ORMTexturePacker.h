#pragma once
#include "AssetTypes/RawImage.h"

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

    bool LoadImageToRaw(const std::string& filepath, RawImage& outImage);
    uint8 GetPixelChannel(const RawImage& img, int32 px, int32 py);
};