#include "GraphicsPch.h"
#include "GraphicsProgram.h"
#include "Shader/Shader.h"

namespace MGF3D
{
	GraphicsProgram::GraphicsProgram() = default;
	GraphicsProgram::~GraphicsProgram() = default;

	GraphicsProgramPtr GraphicsProgram::Create(const Vector<ShaderPtr>& shaders)
	{
		auto program = GraphicsProgramPtr(new GraphicsProgram());
		if (!program->Link(shaders)) return nullptr;
		return program;
	}
}