#include "CoreUtils.h"
#pragma once

namespace MGF3D
{
	template<typename T>
	inline constexpr bool CoreUtils::IsBetween(T value, T min, T max)
	{
		return (value >= min && value <= max);
	}

	template<typename T>
	inline constexpr bool CoreUtils::IsInRange(T value, T min, T max)
	{
		return (value >= min && value < max);
	}

	template<typename T>
	inline constexpr const T& MGF3D::CoreUtils::Select(bool condition, const T& a, const T& b)
	{
		return condition ? a : b;
	}
}