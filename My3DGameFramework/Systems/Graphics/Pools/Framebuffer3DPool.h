#pragma once
#include "Pools/FramebufferPool.h"
#include "Framebuffers/GLFramebuffer3D.h"

namespace MGF3D
{
	class Framebuffer3DPool : public FramebufferPool<GLFramebuffer3D>
	{
		using Super = FramebufferPool<GLFramebuffer3D>;

	public:
		Framebuffer3DPool(uint32 maxCapacity = 50);
		virtual ~Framebuffer3DPool();

	public:
		GLFramebuffer3DPtr Request
		(
			uint32 width, uint32 height, uint32 depth,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);
	};
}