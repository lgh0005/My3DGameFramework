#pragma once
#include "Parsers/Parser.h"

CLASS_PTR(JsonParser)
class JsonParser : public Parser
{
public:
	virtual ~JsonParser() override;
	virtual bool LoadFromFile(const std::string& path) override;
	virtual void Clear() override;

protected:
	JsonParser();
	bool ValidateRoot(const std::string& rootKey);
	nlohmann::json m_jsonData;
};