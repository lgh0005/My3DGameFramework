#pragma once

namespace MGF3D
{
    struct alignas(16) MaterialData
    {
        vec4 albedoFactor;

        vec3 emissiveFactor;
        float emissionStrength;

        float shininess;
        float heightScale;
        float metallicFactor;
        float roughnessFactor;
    };
}