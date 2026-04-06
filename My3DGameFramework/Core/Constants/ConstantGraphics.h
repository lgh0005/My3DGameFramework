#pragma once

namespace MGF3D
{
	// max light and shadow caster count per scene
	static constexpr MAX_LIGHTS				32;
	static constexpr MAX_SHADOW_CASTER		8;

	// max bone influence values
	static constexpr MAX_BONES				100;
	static constexpr MAX_BONE_INFLUENCE		4;

	// default ssao values
	static constexpr SSAO_KERNEL_SIZE		64;
	static constexpr SSAO_NOISE_DIM			4;
}