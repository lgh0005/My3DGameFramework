#pragma once

template<typename T>
inline constexpr T Utils::Min(T a, T b) noexcept
{
	static_assert(std::is_arithmetic<T>::value);
	return glm::min<T>(a, b);
}

template<typename T>
inline constexpr T Utils::Max(T a, T b) noexcept
{
	static_assert(std::is_arithmetic<T>::value);
	return glm::max<T>(a, b);
}