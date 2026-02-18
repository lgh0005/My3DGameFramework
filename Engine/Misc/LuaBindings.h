#pragma once
#include <sol/sol.hpp>

class LuaBindings
{
	DECLARE_STATIC_CLASS(LuaBindings)

public:
	static void Bind(sol::state& lua);


private:
	static void BindMath(sol::state& lua);
	static void BindCore(sol::state& lua);
	static void BindComponents(sol::state& lua);
};