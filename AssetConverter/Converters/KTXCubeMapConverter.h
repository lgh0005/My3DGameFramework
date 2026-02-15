#pragma once

struct CubeFaceElement 
{
    std::string path;
    bool flipY = false;
    bool flipX = false;
};

class KTXCubeMapConverter
{
	DECLARE_SINGLE(KTXCubeMapConverter)

public:
    bool Convert
    (
        const std::vector<CubeFaceElement>& faces, 
        const std::string& outputPath,
        bool isSRGB = true
    );

private:
    bool PrepareTexture
    (
        const std::vector<CubeFaceElement>& faceConfigs, 
        ktxTexture2** outTexture, 
        int32& outWidth, int32& outHeight,
        bool isSRGB = true
    );

    bool UploadFaceData
    (
        const std::vector<CubeFaceElement>& faceConfigs, 
        ktxTexture2* texture, 
        int32 width, int32 height
    );

    void FlipImageHorizontal
    (
        unsigned char* data, 
        int32 width, int32 height, 
        int32 channels
    );

    bool SaveToFile(ktxTexture2* texture, const std::string& outputPath);
};