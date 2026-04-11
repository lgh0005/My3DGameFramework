#pragma once

namespace MGF3D
{
	// max light and shadow caster count per scene
	static constexpr int32 MAX_LIGHTS		  { 32 };
	static constexpr int32 MAX_SHADOW_CASTER  { 8 };

	// max bone influence values
	static constexpr int32 MAX_BONES		  { 100 };
	static constexpr int32 MAX_BONE_INFLUENCE { 4 };

	// default ssao values
	static constexpr int32 SSAO_KERNEL_SIZE	  { 64 };
	static constexpr int32 SSAO_NOISE_DIM	  { 4 };
}