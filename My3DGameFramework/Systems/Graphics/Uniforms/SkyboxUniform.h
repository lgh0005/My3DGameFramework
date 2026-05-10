#pragma once

namespace MGF3D
{
    struct alignas(16) SkyboxData
    {
        mat4 view;
        mat4 projection;
        vec4 tint;
        float intensity;
        float rotation;
        float padding[2];
    };
}