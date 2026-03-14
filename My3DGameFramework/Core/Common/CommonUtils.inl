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
	inline void Swap(T& a, T& b)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template<typename T>
	inline void CommonUtils::MemZero(T& value)
	{
		std::memset(&value, 0, sizeof(T));
	}

	inline void CommonUtils::Memset(void* dest, int value, size_t size)
	{
		std::memset(dest, value, size);
	}
}