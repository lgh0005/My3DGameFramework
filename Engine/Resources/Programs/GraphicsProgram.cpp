#include "EnginePch.h"
#include "GraphicsProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(GraphicsProgram)

GraphicsProgramPtr GraphicsProgram::Load(const GraphicsProgramDesc& desc)
{
	// 1. 셰이더 리소스 생성 (Vertex, Fragment)
	ShaderPtr vs = Shader::CreateFromFile(desc.vsPath, GL_VERTEX_SHADER);
	ShaderPtr fs = Shader::CreateFromFile(desc.fsPath, GL_FRAGMENT_SHADER);
	if (!vs || !fs)
	{
		LOG_ERROR("GraphicsProgram: Failed to create shaders. VS: {}, FS: {}", desc.vsPath, desc.fsPath);
		return nullptr;
	}

	// 2. 프로그램 생성 및 링크
	GraphicsProgramPtr program = Create({ vs, fs });
	if (program)
	{
		program->SetName(desc.name);
		// TODO : 이것도 리소스마다 path를 따로 선언하도록 만들 필요가 있음
		program->SetPath(desc.vsPath + "|" + desc.fsPath);
	}

	return program;
}

GraphicsProgramPtr GraphicsProgram::Create(const std::vector<ShaderPtr>& shaders)
{
	auto program = GraphicsProgramPtr(new GraphicsProgram());
	if (!program->Link(shaders)) return nullptr;
	return program;
}