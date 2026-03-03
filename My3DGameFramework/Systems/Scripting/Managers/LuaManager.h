#pragma once
#include <sol/sol.hpp>

namespace MGF3D
{
	class LuaManager
	{
		DECLARE_SINGLE(LuaManager)
		DECLARE_NONINSTANTIABLE(LuaManager)

	public:
		void Init();
		void Clear();

		void CollectLuaGarbage();
		sol::state& GetLuaVM() { return m_luaVirtualMachine; }

	private:
		sol::state m_luaVirtualMachine;

		void BindEngineAPI();
	};
}