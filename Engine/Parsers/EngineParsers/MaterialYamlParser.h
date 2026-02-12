#pragma once
#include "Parsers/YamlParser.h"

CLASS_PTR(MaterialYamlParser)
class MaterialYamlParser : public YamlParser
{
public:
	virtual ~MaterialYamlParser();
	virtual bool LoadConfig(const std::string& configPath) override;
	bool LoadMaterial(Material* outMaterial);

private:
	MaterialYamlParser();
	TexturePtr LoadTextureInternal(std::string_view tokenKey);
};