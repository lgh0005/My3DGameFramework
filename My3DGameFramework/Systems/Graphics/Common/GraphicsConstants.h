#pragma once

namespace MGF3D
{
	constexpr uint64 StaticPoolSize = 128ULL * 1024ULL * 1024ULL;
	constexpr uint64 DynamicPoolSize = 64ULL * 1024ULL * 1024ULL;

	static constexpr usize MAX_BONE_INFLUENCE		{ 4 };
	static constexpr usize MAX_LIGHTS				{ 32 };
	static constexpr usize OPENGL_MEMORY_ALIGNMENT	{ 16 };
}