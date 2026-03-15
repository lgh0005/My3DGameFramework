#pragma once
#include <array>
#include <bitset>

namespace MGF3D
{
	/*==========================//
	//    std::array 단순 래핑   //
	//==========================*/
	template<typename T, size_t N>
	using Array = std::array<T, N>;

	/*===========================//
	//    std::bitset 단순 래핑   //
	//===========================*/
	template<size_t N>
	using Bitset = std::bitset<N>;
}