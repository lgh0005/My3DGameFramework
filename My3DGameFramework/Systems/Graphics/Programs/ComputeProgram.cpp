#include "GraphicsPch.h"
#include "ComputeProgram.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	ComputeProgram::ComputeProgram(StringView name) : Super(name) { }
	ComputeProgram::~ComputeProgram() = default;

	ComputeProgramPtr ComputeProgram::Create(StringView name)
	{
		auto program = SharedPtr<ComputeProgram>(new ComputeProgram(name));
		program->SetState(EResourceState::Loaded);
		MGF_LOG_INFO("ComputeProgram '{}' created. Waiting for compute shader sync.", name.data());
		return program;
	}

	/*========================//
	//  ComputeProgram Type   //
	//========================*/
	int16 ComputeProgram::s_typeIndex = -1;
	const MGFType* ComputeProgram::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void ComputeProgram::Dispatch(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ) const
	{
		if (m_state != EResourceState::Ready) return;
		Use();
		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	}

	void ComputeProgram::Wait(uint32 barrierBit) const
	{
		glMemoryBarrier(barrierBit);
	}
}