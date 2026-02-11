#pragma once
#include "Parsers/JsonParser.h"

CLASS_PTR(LuaParser)
class LuaParser : public JsonParser
{
public:
	virtual ~LuaParser() override;

	virtual bool LoadConfig(const std::string& configPath) override;
	virtual bool LoadFromFile(const std::string& path) override;
	virtual void Clear() override;

	// 루아 전용 로직: 활성화된 모듈 목록이나 전역 변수 후보를 반환하는 기능 등
	const std::vector<std::string>& GetActiveModules() const { return m_activeModules; }

private:
	LuaParser();
	void ExtractLuaSettings();

	std::vector<std::string> m_activeModules;
};