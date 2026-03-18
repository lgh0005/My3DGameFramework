#include "GraphicsPch.h"
#include "LightArrayUBO.h"

namespace MGF3D
{
    // 정확한 크기 및 정렬 검증
    MGF_ASSERT_GPU_EXACT_SIZE(LightArrayUBO, 4112);
    MGF_ASSERT_GPU_ALIGNMENT(LightArrayUBO);

    // 멤버 변수의 정확한 오프셋 검증
    MGF_ASSERT_GPU_EXACT_OFFSET(LightArrayUBO, viewPos, 4096);
    MGF_ASSERT_GPU_EXACT_OFFSET(LightArrayUBO, lightCount, 4108);
}