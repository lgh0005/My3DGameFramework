#pragma once

namespace MGF3D
{
    struct alignas(16) SpotShadowData
    {
        mat4 lightSpaceMatrix;
        int32 shadowMapIdx;
        float shadowBias;
        int32 pad[2];
    };
}