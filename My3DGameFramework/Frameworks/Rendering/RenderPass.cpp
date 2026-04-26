#include "FrameworkPch.h"
#include "RenderPass.h"
#include "Rendering/RenderContext.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	RenderPass::RenderPass() = default;
	RenderPass::~RenderPass() = default;

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 RenderPass::s_typeIndex = -1;
	const MGFType* RenderPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}