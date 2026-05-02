#pragma once

namespace MGF3D
{
    // Spot Light (64 Bytes)
    struct alignas(16) SpotLightData
    {
        vec4 position;      // w: Range
        vec4 direction;
        vec4 color;
        vec4 params;        // x: InnerCutoff, y: OuterCutoff
        int32 shadowIndex;
        int32 pad[3];
    };
}