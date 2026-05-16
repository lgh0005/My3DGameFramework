#pragma once

namespace MGF3D
{
    class ORMTexturePacker
    {
        MGF_DECLARE_SINGLE(ORMTexturePacker)

    private:
        ORMTexturePacker();
        ~ORMTexturePacker();

    public:
        bool Convert
        (
            const String& aoPath,
            const String& roughPath,
            const String& metalPath,
            const String& outPngPath,
            bool invertRoughness = false,
            bool flipY = true
        );

        bool Pack
        (
            const String& aoPath,
            const String& roughPath,
            const String& metalPath,
            RawImage& outImage,
            bool invertRoughness = false,
            bool flipY = true
        );

    private:

        bool LoadImageToRaw
        (
            const String& filepath,
            RawImage& outImage,
            bool flipY = true
        );

        void FlipImageVertically(uint8* data, int32 w, int32 h, int32 ch);

        uint8 GetPixelChannel
        (
            const RawImage& img,
            int32 px, int32 py,
            int32 channelOffset
        );
    };
}