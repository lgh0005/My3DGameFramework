#include "Utils.h"
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

inline constexpr uint32 Utils::StrHash(const char* str)
{
	uint32 hash = 2166136261u; // FNV_offset_basis
	while (*str)
	{
		hash ^= (uint8_t)*str++;
		hash *= 16777619u;     // FNV_prime
	}
	return hash;
}

inline constexpr uint32 Utils::StrHash(const std::string& str)
{
	return StrHash(str.c_str());
}
