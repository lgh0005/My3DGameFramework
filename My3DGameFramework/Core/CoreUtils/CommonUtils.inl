#include "CommonUtils.h"
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

	template<typename T>
	inline T CommonUtils::Select(bool condition, const Func<T>& trueFunc, const Func<T>& falseFunc)
	{
		if (condition) return trueFunc ? trueFunc() : T();
		else return falseFunc ? falseFunc() : T();
	}

	template<typename T>
	inline constexpr void CommonUtils::Swap(T& a, T& b)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}
}