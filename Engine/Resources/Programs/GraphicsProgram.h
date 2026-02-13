#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Programs/Program.h"

CLASS_PTR(GraphicsProgram)
class GraphicsProgram : public Program
{
	DEFINE_RESOURCE_TYPE(ResourceType::GraphicsProgram, GraphicsProgramDesc)

public:
	virtual ~GraphicsProgram();
	static GraphicsProgramPtr Load(const GraphicsProgramDesc& desc);

private:
	GraphicsProgram();
	static GraphicsProgramPtr Create(const std::vector<ShaderPtr>& shaders);
};