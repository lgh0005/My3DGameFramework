#include "GraphicsPch.h"
#include "ComputeProgram.h"
#include "Shader/Shader.h"

namespace MGF3D
{
	ComputeProgram::ComputeProgram() = default;
	ComputeProgram::~ComputeProgram() = default;

	ComputeProgramPtr ComputeProgram::Create(const ShaderPtr& computeShader)
	{
		auto program = ComputeProgramPtr(new ComputeProgram());
		if (!program->Link({ computeShader })) return nullptr;
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
}