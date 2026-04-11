#pragma once
#include "Programs/Program.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)

	MGF_CLASS_PTR(ComputeProgram)
	class ComputeProgram : public Program
	{
	public:
		virtual ~ComputeProgram() override;
		static ComputeProgramPtr Create(const ShaderPtr& computeShader);

	public:
		// 실행 (Dispatch)
		// 컴퓨트 셰이더의 핵심입니다. x, y, z 그룹 개수를 지정합니다.
		void Dispatch(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ) const;

		// 메모리 장벽 (Memory Barrier)
		// SSBO나 ImageStore 작업 후, 다음 단계에서 안전하게 읽기 위해 대기할 때 씁니다.
		void Wait(uint32 barrierBit = GL_ALL_BARRIER_BITS) const;

	private:
		ComputeProgram();
	};
}