#pragma once
#include <array>

namespace MGF3D
{
	/*==========================//
	//    std::array 단순 래핑   //
	//==========================*/
	template<typename T, size_t N>
	using Array = std::array<T, N>;
}