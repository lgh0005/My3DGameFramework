#pragma once

namespace MGF3D
{
	class CommonUtils
	{
		MGF_DECLARE_UTILITIES(CommonUtils)

			/*=======================//
			//   constexpr methods   //
			//=======================*/
	public:
		template<typename T> static constexpr bool IsBetween(T value, T min, T max);
		template<typename T> static constexpr bool IsInRange(T value, T min, T max);
		template<typename T> static constexpr const T& Select(bool condition, const T& a, const T& b);
		template<typename T> static constexpr void Swap(T& a, T& b);

		/*=====================//
		//   runtime methods   //
		//=====================*/
	public:
		template <typename T> static T Select(bool condition, const Func<T>& trueFunc, const Func<T>& falseFunc);
		static void Select(bool condition, const Action<>& trueAction, const Action<>& falseAction);
	};
}

#include "CoreUtils/CommonUtils.inl"