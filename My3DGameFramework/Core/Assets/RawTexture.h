#pragma once

namespace MGF3D
{
    enum class RawTextureType : uint8
    {
        None = 0, 
        Albedo, 
        Normal, 
        Emissive,
        ORM, 
        AmbientOcclusion, 
        Metallic,
        Roughness, 
        Specular, 
        Height,
        Glossiness, 
        MAX
    };

    struct RawTexture
    {
        String fileName;
        RawTextureType type;
    };
}