#pragma once

namespace MGF3D
{
    struct alignas(16) GlobalData
    {
        mat4 view;         // 64 bytes
        mat4 projection;   // 64 bytes
        vec3 viewPos;      // 12 bytes

        float time;        // 4 bytes

        int32 dirLightCount;     // 4 bytes
        int32 pointLightCount;   // 4 bytes
        int32 spotLightCount;    // 4 bytes
        int32 pad0;              // 4 bytes
    };
}