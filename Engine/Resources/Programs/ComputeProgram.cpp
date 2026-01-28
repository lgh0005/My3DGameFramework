#include "EnginePch.h"
#include "ComputeProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(ComputeProgram)

ComputeProgramUPtr ComputeProgram::Create(const std::string& shaderPath)
{
	ShaderPtr shader = Shader::CreateFromFile(shaderPath, GL_COMPUTE_SHADER);
	if (!shader) return nullptr;
	return Create(shader);
}

ComputeProgramUPtr ComputeProgram::Create(ShaderPtr shader)
{
	auto program = ComputeProgramUPtr(new ComputeProgram());
	if (!program->Link({ shader })) return nullptr;
	return std::move(program);
}

void ComputeProgram::Dispatch(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ) const
{
	Use();
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void ComputeProgram::Wait(uint32 barrierBit) const
{
	// 예: GL_SHADER_STORAGE_BARRIER_BIT, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 등
	glMemoryBarrier(barrierBit);
}