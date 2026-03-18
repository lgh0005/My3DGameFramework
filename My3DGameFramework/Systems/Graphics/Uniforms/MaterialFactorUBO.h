#pragma once

namespace MGF3D
{
    enum class MaterialFlags : uint32
    {
        None          = 0,
        HasORM        = 1 << 0, // 1: ORM 맵 1개 샘플링,    0: 개별 맵 3개 샘플링
        UseGlossiness = 1 << 1  // 1: (1.0 - 값) 반전 적용, 0: 그대로 사용
    };

	struct alignas(OPENGL_MEMORY_ALIGNMENT) MaterialFactorsUBO
	{
        vec4   albedoFactor;
        vec3   emissiveFactor;
        float  shininess;
        float  metallicFactor;
        float  roughnessFactor;
        float  emissionStrength;
        float  heightScale;
        uint32 materialFlag;
        vec3   pad0;
	};
}