#include "EnginePch.h"
#include "LuaParser.h"

LuaParser::LuaParser() : m_luaState(LUA_MGR.GetLuaVM()) {}
LuaParser::~LuaParser() = default;

bool LuaParser::LoadFromLuaScriptFile(const std::string& path)
{
	m_isValid = false;

	auto loadResult = m_luaState.load_file(path);
    if (!loadResult.valid())
    {
        sol::error err = loadResult;
        LOG_ERROR("LuaParser: Load failed - {}. Error: {}", path, err.what());
        return false;
    }

    // 로드된 스크립트 실행
    auto execResult = loadResult();
    if (!execResult.valid())
    {
        sol::error err = execResult;
        LOG_ERROR("LuaParser: Execution failed - {}. Error: {}", path, err.what());
        return false;
    }

    m_isValid = true;
    return true;
}

bool LuaParser::HasFunction(const std::string& funcName)
{
    if (!m_isValid) return false;
    sol::protected_function func = m_luaState[funcName];
    return func.valid();
}

sol::table LuaParser::GetTable(const std::string& tableName)
{
    if (m_isValid)
    {
        // sol::optional을 이용해 테이블이 존재하는지, 그리고 진짜 테이블 타입인지 확인
        sol::optional<sol::table> maybeTable = m_luaState[tableName];
        if (maybeTable) return maybeTable.value();
    }

    return sol::table();
}