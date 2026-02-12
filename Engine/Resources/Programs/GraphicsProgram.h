#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Programs/Program.h"

CLASS_PTR(GraphicsProgram)
class GraphicsProgram : public Program
{
	DEFINE_RESOURCE_TYPE(ResourceType::GraphicsProgram, GraphicsProgramDesc)

public:
	virtual ~GraphicsProgram();
	static GraphicsProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
	static GraphicsProgramUPtr Create(const std::string& vertPath, const std::string& fragPath);

private:
	GraphicsProgram();
};