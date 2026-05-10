#pragma once

namespace MGF3D
{
    struct alignas(16) IBLData
    {
        mat4  VPs[6];
        float roughness;
        float padding[3];
    };
}