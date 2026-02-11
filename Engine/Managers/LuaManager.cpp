#include "EnginePch.h"
#include "LuaManager.h"

void LuaManager::Init()
{
	m_luaVirtualMachine.open_libraries
	(
		sol::lib::base, 
		sol::lib::math, 
		sol::lib::table, 
		sol::lib::string
	);
	BindEngineAPI();
	LOG_INFO("LuaManager Initialized.");
}

void LuaManager::CollectLuaGarbage()
{
	m_luaVirtualMachine.collect_garbage();
}

void LuaManager::BindEngineAPI()
{
	// Config/LuaScriptingConfig.json에 있는 문자열 토큰들을 이용한 스크립팅을 허용
	// TODO: 이후 GameObject, Transform, Input 등을 여기에 추가 등록
}

void LuaManager::Clear()
{
	CollectLuaGarbage();
}