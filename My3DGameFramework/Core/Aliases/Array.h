#pragma once
#include <array>
#include <bitset>

namespace MGF3D
{
	// TODO : 이후에는 진지하게 class로 래핑을 해둬야함.

	/*==========================//
	//    std::array 단순 래핑   //
	//==========================*/
	template<typename T, size_t N>
	using Array = std::array<T, N>;
}