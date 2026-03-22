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

		// TODO : 런타임의 Select, if-else에 따른 함수 실행 등도 추가 필요

		template<typename T>
		static constexpr void Swap(T& a, T& b);
	};
}

#include "Common/CommonUtils.inl"