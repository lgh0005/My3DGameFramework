#pragma once
#include "Pools/FramebufferPool.h"
#include "Framebuffers/GLFramebufferCube.h"

namespace MGF3D
{
	class FramebufferCubePool : public FramebufferPool<GLFramebufferCube>
	{
		using Super = FramebufferPool<GLFramebufferCube>;

	public:
		FramebufferCubePool(uint32 maxCapacity = 50);
		virtual ~FramebufferCubePool();

	public:
		GLFramebufferCubePtr Request
		(
			uint32 size,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);
	};
}