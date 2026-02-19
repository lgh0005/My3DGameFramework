#pragma once

namespace MGF3D
{
	class Math
	{
		DECLARE_UTILITIES(Math)

	public:
		static float Lerp(float a, float b, float f);
		template<typename T> static constexpr T Min(const T& a, const T& b) noexcept;
		template<typename T> static constexpr T Max(const T& a, const T& b) noexcept;
		static bool HasLength(const vec3& v);
		static vec3 Normalize(const vec3& v);
	};
}

#include "Math.inl"