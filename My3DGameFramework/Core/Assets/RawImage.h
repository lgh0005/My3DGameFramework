#pragma once

namespace MGF3D
{
    struct RawImage
    {
        int32 width = 0;
        int32 height = 0;
        int32 channels = 0;
        Vector<uint8> pixels;
        bool IsValid() const;
    };
}