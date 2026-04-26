#include "FrameworkPch.h"
#include "RenderPipeline.h"
#include "Rendering/RenderContext.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	RenderPipeline::RenderPipeline() = default;
	RenderPipeline::~RenderPipeline() = default;

	/*==================================//
	//    RenderPipeline Custom Type    //
	//==================================*/
	int16 RenderPipeline::s_typeIndex = -1;
	const MGFType* RenderPipeline::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPipeline");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}