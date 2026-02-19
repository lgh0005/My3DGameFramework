#include "EnginePch.h"
#include "GraphicsProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(GraphicsProgram)

GraphicsProgramPtr GraphicsProgram::Load(const GraphicsProgramDesc& desc)
{
	// 1. 인스턴스 생성
	auto program = GraphicsProgramPtr(new GraphicsProgram());
	program->m_desc = desc;

	// 2. AddShaderStage를 활용해 조각 수집
	std::vector<ShaderPtr> shaders;

	// 3. 필수 체크 (최소한 VS/FS는 있어야 함)
	if (desc.vsPath.empty() || desc.fsPath.empty())
		return nullptr;

	if (!program->AddShaderStage(desc.vsPath, GL_VERTEX_SHADER, shaders)) return nullptr;
	if (!program->AddShaderStage(desc.fsPath, GL_FRAGMENT_SHADER, shaders)) return nullptr;
	if (!program->AddShaderStage(desc.tcPath, GL_TESS_CONTROL_SHADER, shaders)) return nullptr;
	if (!program->AddShaderStage(desc.tePath, GL_TESS_EVALUATION_SHADER, shaders)) return nullptr;
	if (!program->AddShaderStage(desc.gsPath, GL_GEOMETRY_SHADER, shaders)) return nullptr;

	// 4. 프로그램 생성 및 링크
	if (!program->Link(shaders))
	{
		LOG_ERROR("GraphicsProgram: Link failed for {}", desc.name);
		return nullptr;
	}

	return program;
}