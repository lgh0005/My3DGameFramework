#pragma once

namespace MGF3D
{
	template<typename T>
	struct Rect
	{
		MGF_STATIC_ASSERT
		(
			std::is_arithmetic_v<T>,
			"Rect<T>: T must be an arithmetic type."
		);

		T x0	{ 0 };
		T y0	{ 0 };
		T x1	{ 0 };
		T y1	{ 0 };

		// 생성자
		constexpr Rect();
		constexpr Rect(T _x0, T _y0, T _x1, T _y1);

		// 유틸 메서드
		constexpr T GetWidth()  const;
		constexpr T GetHeight() const;

		// 유효성 검증
		constexpr bool IsValid() const;
	};

	using Recti = Rect<int32>;
	using Rectf = Rect<float>;
}

#include "Spatial/Rect.inl"