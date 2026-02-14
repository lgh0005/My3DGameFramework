#pragma once
#include "Parsers/Parser.h"
#include <yaml-cpp/yaml.h>

CLASS_PTR(YamlParser)
class YamlParser : public Parser
{
public:
	YamlParser();
	virtual ~YamlParser() override;
	virtual bool LoadFromFile(const std::string& path) override;
	virtual void Clear() override;

	bool ValidateRoot(const std::string& rootKey);

/*============================//
//   parsing helper methods   //
//============================*/
protected:
	float       LoadFloat(const YAML::Node& node, std::string_view tokenKey, float defaultValue = 0.0f);
	bool        LoadBool(const YAML::Node& node, std::string_view tokenKey, bool defaultValue = false);
	int         LoadInt(const YAML::Node& node, std::string_view tokenKey, int defaultValue = 0);
	std::string LoadStr(const YAML::Node& node, std::string_view tokenKey, const std::string& defaultValue = "");
	glm::vec2   LoadVec2(const YAML::Node& node, std::string_view tokenKey, const glm::vec2& defaultValue = glm::vec2(0.0f));
	glm::vec3   LoadVec3(const YAML::Node& node, std::string_view tokenKey, const glm::vec3& defaultValue = glm::vec3(0.0f));
	glm::vec4   LoadVec4(const YAML::Node& node, std::string_view tokenKey, const glm::vec4& defaultValue = glm::vec4(0.0f));

protected:
	YAML::Node m_rootNode;
};