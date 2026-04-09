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

public:
	virtual GraphicsProgramDesc& GetDesc() override { return m_desc; }
	virtual const ResourceDesc& GetDesc() const override { return m_desc; }

private:
	GraphicsProgram();
	GraphicsProgramDesc m_desc;
};