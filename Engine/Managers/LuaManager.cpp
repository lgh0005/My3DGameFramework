#include "EnginePch.h"
#include "LuaManager.h"
#include "Resources/LuaScript.h"

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
	// Config/LuaScriptingConfig.json에 있는 문자열 토큰들을 이용한 스크립팅을 허용
	// TODO: 이후 GameObject, Transform, Input 등을 여기에 추가 등록
}

void LuaManager::Clear()
{
	CollectLuaGarbage();
}