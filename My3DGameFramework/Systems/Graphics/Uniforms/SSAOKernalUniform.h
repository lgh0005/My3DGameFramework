#pragma once

namespace MGF3D
{
    struct alignas(16) SSAOKernelData
    {
        vec4 samples[64];
    };
}