#include "EnginePch.h"
#include "ComputeProgram.h"
#include "Graphics/Shader.h"

DECLARE_DEFAULTS_IMPL(ComputeProgram)

ComputeProgramPtr ComputeProgram::Load(const ComputeProgramDesc& desc)
{
	// 1. 객체 생성 (기본 생성자)
	auto program = ComputeProgramPtr(new ComputeProgram());

	// 2. Desc 주입 (Single Source of Truth)
	program->m_desc = desc;

	// 3. 컴퓨트 셰이더 리소스 생성 (m_desc.path 사용)
	ShaderPtr shader = Shader::CreateFromFile(program->m_desc.path, GL_COMPUTE_SHADER);
	if (!shader)
	{
		LOG_ERROR("ComputeProgram: Failed to create compute shader: {}", program->m_desc.path);
		return nullptr;
	}

	// 4. 프로그램 링크
	// 부모 클래스(Program)의 Link는 비정적 함수이므로, 생성된 인스턴스 포인터를 통해 호출합니다.
	if (!program->Link({ shader }))
	{
		LOG_ERROR("ComputeProgram: Link failed for {}", program->GetName());
		return nullptr;
	}

	LOG_INFO("ComputeProgram Loaded: {} (Path: {})", program->GetName(), program->m_desc.path);
	return program;
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