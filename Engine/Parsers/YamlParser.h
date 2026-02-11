#pragma once
#include "Parsers/Parser.h"
#include <yaml-cpp/yaml.h>

CLASS_PTR(YamlParser)
class YamlParser : public Parser
{
public:
	virtual ~YamlParser() override;
	virtual bool LoadFromFile(const std::string& path) override;
	virtual void Clear() override;

protected:
	YamlParser();
	bool ValidateRoot(const std::string& rootKey);

	YAML::Node m_rootNode;
};