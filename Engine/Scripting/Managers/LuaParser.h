#pragma once
#include <sol/sol.hpp>

CLASS_PTR(LuaParser)
class LuaParser
{
public:
	~LuaParser();
	LuaParser();

	bool LoadFromLuaScriptFile(const std::string& path);
	bool HasFunction(const std::string& funcName);
	sol::table GetTable(const std::string& tableName);

	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T());

private:
	sol::state_view m_luaState;
	bool m_isValid = false;
};

#include "Parsers/LuaParser.inl"