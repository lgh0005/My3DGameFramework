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
}