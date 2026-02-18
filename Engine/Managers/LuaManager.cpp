#include "EnginePch.h"
#include "LuaManager.h"
#include "Resources/LuaScript.h"
#include "Misc/LuaBindings.h"

void LuaManager::Init()
{
	// 0. 루아에서 기본적으로 사용할 라이브러리를 오픈
	m_luaVirtualMachine.open_libraries
	(
		sol::lib::base, 
		sol::lib::math, 
		sol::lib::table, 
		sol::lib::string,
		sol::lib::package
	);

	// 1. 패키지 경로 설정
	std::string scriptSearchPath = RESOURCE.ResolvePath("@BuiltIn/Scripts/?.lua;");
	m_luaVirtualMachine["package"]["path"] = scriptSearchPath + m_luaVirtualMachine["package"]["path"].get<std::string>();

	// 2. Global.lua 로드
	auto globalScript = RESOURCE.Get<LuaScript>("Global");
	if (globalScript)
	{
		sol::protected_function_result result = m_luaVirtualMachine.script(globalScript->GetCode());
		if (!result.valid())
		{
			sol::error err = result;
			LOG_ERROR("Failed to execute Global.lua: {}", err.what());
		}
	}
	else
	{
		LOG_ERROR("Could not find ""Global.lua"" in ResourceManager!");
	}

	BindEngineAPI();
	LOG_INFO("LuaManager Initialized.");
}

void LuaManager::CollectLuaGarbage()
{
	m_luaVirtualMachine.collect_garbage();
}

void LuaManager::BindEngineAPI()
{
	LuaBindings::Bind(m_luaVirtualMachine);
}

void LuaManager::Clear()
{
	CollectLuaGarbage();
}