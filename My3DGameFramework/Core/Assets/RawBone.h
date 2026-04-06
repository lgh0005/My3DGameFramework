#pragma once

namespace MGF3D
{
    struct RawBone
    {
        String name;
        int32 index = -1;
        int32 parentIndex = -1;
        mat4 offsetMatrix{ 1.0f };
        mat4 localMatrix{ 1.0f };
        mat4 globalMatrix{ 1.0f };
    };
}