#pragma once

namespace MGF3D
{
	template<typename T>
	inline constexpr bool CommonUtils::IsBetween(T value, T min, T max)
	{
		return (value >= min && value <= max);
	}

	template<typename T>
	inline constexpr bool CommonUtils::IsInRange(T value, T min, T max)
	{
		return (value >= min && value < max);
	}

	template<typename T>
	inline constexpr const T& MGF3D::CommonUtils::Select(bool condition, const T& a, const T& b)
	{
		return condition ? a : b;
	}
}