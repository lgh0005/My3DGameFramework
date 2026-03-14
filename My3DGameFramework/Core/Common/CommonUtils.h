#pragma once

namespace MGF3D
{
	class CommonUtils
	{
		MGF_DECLARE_UTILITIES(CommonUtils)

	public:
		template<typename T>
		static constexpr bool IsBetween(T value, T min, T max);

		template<typename T>
		static constexpr bool IsInRange(T value, T min, T max);

		template<typename T>
		static constexpr const T& Select(bool condition, const T& a, const T& b);

		template<typename T>
		static constexpr void Swap(T& a, T& b);

		template<typename T>
		static void MemZero(T& value);

		static void Memset(void* dest, int32 value, usize size);
	};
}

#include "Common/CommonUtils.inl"