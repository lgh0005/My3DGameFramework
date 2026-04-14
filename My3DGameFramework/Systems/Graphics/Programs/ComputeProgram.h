#pragma once
#include "Programs/Program.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ComputeProgram)
	class ComputeProgram : public Program
	{
	/*========================//
	//  ComputeProgram Type   //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		ComputeProgram();
		virtual ~ComputeProgram() override;

	public:
		// 컴퓨트 셰이더 실행
		void Dispatch(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ) const;

		// 메모리 장벽 설정
		void Wait(uint32 barrierBit = GL_ALL_BARRIER_BITS) const;
	};
}