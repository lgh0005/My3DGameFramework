#pragma once

namespace MGF3D
{
    enum class RawTextureType : uint8
    {
        None = 0,
        Albedo,
        Specular,
        Emissive,
        Normal,
        Height,
        ORM,
        Max
    };

    struct RawTexture
    {
        String fileName;
        RawTextureType type;
    };
}