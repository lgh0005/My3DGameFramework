#pragma once

namespace MGF3D
{
    struct alignas(OPENGL_MEMORY_ALIGNMENT) StaticVertex
    {
        vec3  position   { 0.0f };
        vec3  normal     { 0.0f, 1.0f, 0.0f };
        vec2  texCoord   { 0.0f };
        vec3  tangent    { 1.0f, 0.0f, 0.0f };
        float pad0       { 0.0f };
    };
}