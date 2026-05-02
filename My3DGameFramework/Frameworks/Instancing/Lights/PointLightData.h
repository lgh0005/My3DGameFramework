#pragma once

namespace MGF3D
{
    struct alignas(16) PointLightData
    {
        vec4 position; // w: Range
        vec4 color;
        int32 shadowIndex;
        int32 pad[3];
    };
}