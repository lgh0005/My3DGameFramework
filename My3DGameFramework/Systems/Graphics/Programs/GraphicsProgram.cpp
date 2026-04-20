#include "GraphicsPch.h"
#include "GraphicsProgram.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GraphicsProgram::GraphicsProgram(StringView name) : Super(name) { }
	GraphicsProgram::~GraphicsProgram() = default;

	GraphicsProgramPtr GraphicsProgram::Create(StringView name)
	{
		auto program = SharedPtr<GraphicsProgram>(new GraphicsProgram(name));
		program->SetState(EResourceState::Loaded);
		return program;
	}

	/*========================//
	// GraphicsProgram Type   //
	//========================*/
	int16 GraphicsProgram::s_typeIndex = -1;
	const MGFType* GraphicsProgram::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}