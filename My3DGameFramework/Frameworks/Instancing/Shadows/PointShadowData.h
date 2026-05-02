#pragma once

namespace MGF3D
{
    struct alignas(16) PointShadowData
    {
        float shadowFarPlane;
        int32 shadowMapIdx;
        float shadowBias;
        int32 pad0;
    };
}