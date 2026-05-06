#pragma once
namespace MGF3D
{
    struct alignas(16) SkinnedInstanceData
    {
        mat4 worldMatrix{ 1.0f };
        uint32 boneOffset{ 0 };
        uint32 pad1{ 0 };
        uint32 pad2{ 0 };
        uint32 pad3{ 0 };
    };
}