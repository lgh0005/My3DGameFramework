#include "Rect.h"
#pragma once

namespace MGF3D
{
	template<typename T> inline constexpr Rect<T>::Rect() = default;
	
	template<typename T>
	inline constexpr MGF3D::Rect<T>::Rect(T _x0, T _y0, T _x1, T _y1)
		: x0(_x0), y0(_y0), x1(_x1), y1(_y1) { }

	template<typename T>
	inline constexpr T Rect<T>::GetWidth() const
	{
		return x1 - x0;
	}

	template<typename T>
	inline constexpr T Rect<T>::GetHeight() const
	{
		return y1 - y0;
	}

	template<typename T>
	inline constexpr bool Rect<T>::IsValid() const
	{
		return (x1 > x0) && (y1 > y0);
	}
}