#pragma once

namespace MGF3D
{
    // Unsigned Normalized (8-bit)
    static constexpr uint32 VK_FORMAT_R8_UNORM{ 9 };
    static constexpr uint32 VK_FORMAT_R8G8B8A8_UNORM{ 37 };
    static constexpr uint32 VK_FORMAT_R8G8B8A8_SRGB{ 43 };

    // Floating Point (16-bit)
    static constexpr uint32 VK_FORMAT_R16G16_SFLOAT{ 83 };
    static constexpr uint32 VK_FORMAT_R16G16B16A16_SFLOAT{ 97 };

    // Floating Point (32-bit)
    static constexpr uint32 VK_FORMAT_R32G32B32A32_SFLOAT{ 109 };

    // Depth & Stencil
    static constexpr uint32 VK_FORMAT_D32_SFLOAT{ 126 };
    static constexpr uint32 VK_FORMAT_D24_UNORM_S8_UINT{ 129 };
    static constexpr uint32 VK_FORMAT_D32_SFLOAT_S8_UINT{ 130 };
}