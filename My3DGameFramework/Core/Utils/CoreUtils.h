#pragma once

namespace MGF3D
{
	class CoreUtils
	{
		DECLARE_UTILITIES(CoreUtils)

	public:
		template<typename T>
		static constexpr bool IsBetween(T value, T min, T max);

		template<typename T>
		static constexpr bool IsInRange(T value, T min, T max);
	};
}

#include "Utils/CoreUtils.inl"