#pragma once
#include "Pools/FramebufferPool.h"
#include "Framebuffers/GLFramebuffer2D.h"

namespace MGF3D
{
	class Framebuffer2DPool : public FramebufferPool<GLFramebuffer2D>
	{
		using Super = FramebufferPool<GLFramebuffer2D>;

	public:
		Framebuffer2DPool(uint32 maxCapacity = 50);
		virtual ~Framebuffer2DPool();

	public:
		GLFramebuffer2DPtr Request
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);
	};
}