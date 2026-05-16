#include "RuntimeModule.h"
#include "MGFPostProcessingPass.h"
#include "Managers/TypeManager.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	MGFPostProcessingPass::MGFPostProcessingPass() = default;
	MGFPostProcessingPass::~MGFPostProcessingPass() = default;

	MGFPostProcessingPassUPtr MGFPostProcessingPass::Create()
	{
		auto renderPass = MGFPostProcessingPassUPtr(new MGFPostProcessingPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFPostProcessingPass::Init()
	{
		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFPostProcessingPass::s_typeIndex = -1;
	const MGFType* MGFPostProcessingPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFPostProcessingPass::Execute(RenderContext* context)
	{

	}
}