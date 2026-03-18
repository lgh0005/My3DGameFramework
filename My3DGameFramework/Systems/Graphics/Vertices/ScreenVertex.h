#pragma once

namespace MGF3D
{
    struct alignas(GL_MEMORY_ALIGNMENT) ScreenVertex
    {
        vec3 position   { 0.0f };
        vec2 texCoord   { 0.0f };
        vec3 pad0       { 0.0f };
    };
}