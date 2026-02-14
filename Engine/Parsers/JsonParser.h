#pragma once
#include "Parsers/Parser.h"

CLASS_PTR(JsonParser)
class JsonParser : public Parser
{
public:
	JsonParser();
	virtual ~JsonParser() override;
	virtual bool LoadFromFile(const std::string& path) override;
	virtual void Clear() override;
	const nlohmann::json& GetJsonData() const { return m_jsonData; }
	bool ValidateRoot(const std::string& rootKey);

protected:
	nlohmann::json m_jsonData;
};