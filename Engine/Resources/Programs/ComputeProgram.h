#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Programs/Program.h"

CLASS_PTR(ComputeProgram)
class ComputeProgram : public Program
{
	DEFINE_RESOURCE_TYPE(ResourceType::ComputeProgram, ComputeProgramDesc)

public:
	virtual ~ComputeProgram() override;

	static ComputeProgramPtr Load(const ComputeProgramDesc& desc);

	// 실행 (Dispatch)
	// 컴퓨트 셰이더의 핵심입니다. x, y, z 그룹 개수를 지정합니다.
	void Dispatch(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ) const;

	// 메모리 장벽 (Memory Barrier)
	// SSBO나 ImageStore 작업 후, 다음 단계에서 안전하게 읽기 위해 대기할 때 씁니다.
	void Wait(uint32 barrierBit = GL_ALL_BARRIER_BITS) const;

private:
	ComputeProgram();
	static ComputeProgramUPtr Create(ShaderPtr shader);
};