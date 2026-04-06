#pragma once
#include <functional>

namespace MGF3D
{
	// 반환값이 없는 함수 (C#의 Action)
	template<typename... Args> using Action = std::function<void(Args...)>;

	// 반환값이 있는 함수 (C#의 Func)
	template<typename R, typename... Args> using Func = std::function<R(Args...)>;
}