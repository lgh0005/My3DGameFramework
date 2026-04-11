#pragma once

namespace MGF3D
{
    struct alignas(16) CameraData
    {
        mat4 view;         // 64 bytes
        mat4 projection;   // 64 bytes
        vec3 viewPos;      // 12 bytes
        float pad0;        // 4 bytes (총 144 bytes)
    };
}