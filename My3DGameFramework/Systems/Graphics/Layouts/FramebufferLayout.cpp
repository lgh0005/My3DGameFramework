#include "GraphicsPch.h"
#include "FramebufferLayout.h"

namespace MGF3D
{
	bool FramebufferLayout::IsMultiSampled() const
	{
		return samples > 1;
	}

	bool FramebufferLayout::HasDepthStencil() const
	{
		return depthStencilFormat != 0;
	}
}