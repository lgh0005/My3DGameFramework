#include "GraphicsPch.h"
#include "LightUBO.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_EXACT_SIZE(LightUBO, 128);
    MGF_ASSERT_GPU_ALIGNMENT(LightUBO);
    MGF_ASSERT_GPU_EXACT_OFFSET(LightUBO, diffuse, 80);
    MGF_ASSERT_GPU_EXACT_OFFSET(LightUBO, specular, 96);
}