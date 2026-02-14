#pragma once
#include "Parsers/YamlParser.h"

CLASS_PTR(MaterialYamlParser)
class MaterialYamlParser : public YamlParser
{
public:
	MaterialYamlParser();
	virtual ~MaterialYamlParser();
	bool LoadConfig(const std::string& configPath);
	bool LoadMaterial(Material* outMaterial);

private:
	TexturePtr LoadTextureInternal(std::string_view tokenKey);
};