#include "EnginePch.h"
#include "ComputeProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(ComputeProgram)

ComputeProgramPtr ComputeProgram::Load(const ComputeProgramDesc& desc)
{
	// 1. 컴퓨트 셰이더 리소스 생성
	ShaderPtr shader = Shader::CreateFromFile(desc.path, GL_COMPUTE_SHADER);
	if (!shader)
	{
		LOG_ERROR("ComputeProgram: Failed to create compute shader: {}", desc.path);
		return nullptr;
	}

	// 2. 프로그램 생성 및 링크
	ComputeProgramPtr program = Create(shader);
	if (program)
	{
		// 3. 리소스 메타데이터 설정
		program->SetName(desc.name);
		program->SetPath(desc.path);
	}

	return program;
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
	// SSBO나 Image Load/Store 작업 시 데이터 동기화를 보장합니다.
	glMemoryBarrier(barrierBit);
}