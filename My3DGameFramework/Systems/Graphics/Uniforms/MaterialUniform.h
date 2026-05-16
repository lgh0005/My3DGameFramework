#pragma once

namespace MGF3D
{
    struct alignas(16) MaterialData
    {
        // Chunk 1
        vec4 albedoFactor;

        // Chunk 2
        vec3 emissiveFactor;
        float emissionStrength;

        // Chunk 3
        float heightScale;
        float metallicFactor;
        float roughnessFactor;
        float pad;
    };
}