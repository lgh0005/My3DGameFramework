#pragma once

namespace MGF3D
{
    class KTXCubeMapConverter
    {
        MGF_DECLARE_SINGLE(KTXCubeMapConverter)

    private:
        KTXCubeMapConverter();
        ~KTXCubeMapConverter();

    public:
        bool Convert
        (
            const Vector<CubeFaceElement>& faces,
            const String& outputPath,
            bool isSRGB = true
        );

    private:
        bool PrepareTexture
        (
            const Vector<CubeFaceElement>& faceConfigs,
            ktxTexture2** outTexture,
            int32& outWidth, int32& outHeight,
            bool isSRGB = true
        );

        bool UploadFaceData
        (
            const Vector<CubeFaceElement>& faceConfigs,
            ktxTexture2* texture,
            int32 width, int32 height
        );

        void FlipImageHorizontal
        (
            unsigned char* data,
            int32 width, int32 height,
            int32 channels
        );

        bool SaveToFile(ktxTexture2* texture, const String& outputPath);
    };
}