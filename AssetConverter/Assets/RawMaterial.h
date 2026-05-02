#pragma once
#include "Assets/RawTexture.h"

namespace MGF3D
{
    struct RawMaterial
    {
        String name;
        vec4 albedoFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
        vec3 emissiveFactor = { 0.0f, 0.0f, 0.0f };
        float emissiveStrength = 0.0f;
        float metallicFactor = 0.0f;
        float roughnessFactor = 1.0f;
        Vector<RawTexture> textures;
        String GetTexturePath(RawTextureType type) const;
    };
}