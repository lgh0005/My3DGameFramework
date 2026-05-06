#pragma once

namespace MGF3D
{
    enum class RawTextureType : uint8
    {
        None = 0,
        Albedo = 1,
        Specular = 2,
        Emissive = 3,
        Normal = 4,
        Height = 5,
        ORM = 6,

        AmbientOcclusion = 7,
        Metallic = 8,
        Roughness = 9,
        Glossiness = 10,

        Max
    };

    struct RawTexture
    {
        String fileName;
        RawTextureType type;
    };
}