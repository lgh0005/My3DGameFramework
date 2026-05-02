#pragma once

namespace MGF3D
{
    // Directional Shadow (CSM 전용 - 288 Bytes)
    struct alignas(16) DirectionalShadowData
    {
        mat4 lightSpaceMatrices[4];
        vec4 cascadeSplits;
        int32 shadowMapBaseIdx;
        float shadowBias;
        int32 pad[2];
    };
}