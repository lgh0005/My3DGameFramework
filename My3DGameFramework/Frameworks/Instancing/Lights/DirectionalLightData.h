#pragma once

namespace MGF3D
{
    struct alignas(16) DirectionalLightData
    {
        vec4 direction;
        vec4 color;
        int32 shadowIndex;
        int32 pad[3];
    };
}