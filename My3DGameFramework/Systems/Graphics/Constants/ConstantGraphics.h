#pragma once

namespace MGF3D
{
	// max light and shadow caster count per scene
	static constexpr int32 MAX_LIGHTS		  { 32 };
	static constexpr int32 MAX_SHADOW_CASTER  { 8 };

	// Shadow Map Resolution
	static constexpr int32 SHADOW_MAP_RES_DEFAULT{ 1024 };
	static constexpr int32 SHADOW_MAP_RES_HIGH{ 2048 };
	static constexpr int32 MAX_DIR_SHADOW_LAYERS  { 4 };
	static constexpr int32 MAX_SPOT_SHADOW_LAYERS { MAX_SHADOW_CASTER };
	static constexpr int32 MAX_POINT_SHADOW_COUNT { MAX_SHADOW_CASTER };

	// max bone influence values
	static constexpr int32 MAX_BONES		  { 100 };
	static constexpr int32 MAX_BONE_INFLUENCE { 4 };

	// default ssao values
	static constexpr int32 SSAO_KERNEL_SIZE	  { 64 };
	static constexpr int32 SSAO_NOISE_DIM	  { 4 };
}