#include "GraphicsPch.h"
#include "MaterialFactorUBO.h"

namespace MGF3D
{
    // 1. 전체 크기 및 정렬 검증
    MGF_ASSERT_GPU_EXACT_SIZE(MaterialFactorsUBO, 64);
    MGF_ASSERT_GPU_ALIGNMENT(MaterialFactorsUBO);

    // 2. 멤버 변수의 정확한 오프셋 검증
    MGF_ASSERT_GPU_EXACT_OFFSET(MaterialFactorsUBO, emissiveFactor, 16);
    MGF_ASSERT_GPU_EXACT_OFFSET(MaterialFactorsUBO, shininess, 28);
    MGF_ASSERT_GPU_EXACT_OFFSET(MaterialFactorsUBO, metallicFactor, 32);
    MGF_ASSERT_GPU_EXACT_OFFSET(MaterialFactorsUBO, materialFlag, 48);
}