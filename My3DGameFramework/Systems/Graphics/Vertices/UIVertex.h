#pragma once

namespace MGF3D
{
    struct alignas(GL_MEMORY_ALIGNMENT) UIVertex
    {
        vec3  position    { 0.0f };
        float pad0        { 0.0f };
        vec4  color       { 1.0f };
        vec2  texCoord    { 0.0f };
        vec2  pad1        { 0.0f };
    };
}