#pragma once
#include <functional>

namespace MGF3D
{
	/*=================================//
	//    High-level (std::function)   //
	//=================================*/
	// 반환값이 없는 함수 (C#의 Action)
	template<typename... Args> 
	using Action = std::function<void(Args...)>;

	// 반환값이 있는 함수 (C#의 Func)
	template<typename R, typename... Args> 
	using Func = std::function<R(Args...)>;

	/*=================================//
	//    Low-level (Pure Pointer)     //
	//=================================*/
	// 반환값이 없는 순수 함수 포인터 (C언어 함수 포인터)
	template <typename... Args>
	using PAction = void(*)(Args...);

	// 반환값이 있는 순수 함수 포인터 (C언어 함수 포인터)
	template <typename R, typename... Args>
	using PFunc = R(*)(Args...);
}