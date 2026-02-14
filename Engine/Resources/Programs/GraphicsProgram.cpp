#include "EnginePch.h"
#include "GraphicsProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(GraphicsProgram)

GraphicsProgramPtr GraphicsProgram::Load(const GraphicsProgramDesc& desc)
{
	// 1. 인스턴스 생성 (이제 'program'이라는 인스턴스가 존재함)
	auto program = GraphicsProgramPtr(new GraphicsProgram());

	// 2. Desc 주입
	program->m_desc = desc;

	// 1. 셰이더 리소스 생성 (Vertex, Fragment)
	ShaderPtr vs = Shader::CreateFromFile(desc.vsPath, GL_VERTEX_SHADER);
	ShaderPtr fs = Shader::CreateFromFile(desc.fsPath, GL_FRAGMENT_SHADER);
	if (!vs || !fs)
	{
		LOG_ERROR("GraphicsProgram: Failed to create shaders. VS: {}, FS: {}", desc.vsPath, desc.fsPath);
		return nullptr;
	}

	// 2. 프로그램 생성 및 링크
	if (!program->Link({ vs, fs }))
	{
		LOG_ERROR("GraphicsProgram: Link failed for {}", desc.name);
		return nullptr;
	}

	return program;
}