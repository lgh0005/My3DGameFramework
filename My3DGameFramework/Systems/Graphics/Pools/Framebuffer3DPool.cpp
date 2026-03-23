#include "GraphicsPch.h"
#include "Pools/Framebuffer3DPool.h"
#include "Hashing/FramebufferHash.h"

namespace MGF3D
{
	Framebuffer3DPool::Framebuffer3DPool(uint32 maxCapacity) : Super(maxCapacity) { }
	Framebuffer3DPool::~Framebuffer3DPool() = default;

	GLFramebuffer3DPtr Framebuffer3DPool::Request
	(
		uint32 width, uint32 height, uint32 depth,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples
	)
	{
		// 1. 3D 텍스처 규격에 맞추어 depth 값을 포함하여 해시 생성
		usize hash = FramebufferHash(width, height, colorFormats, depthStencilFormat, samples, depth).value;

		// 2. 가용 리스트 검색
		auto fbo = FindAvailable(hash);
		if (fbo != nullptr) return fbo;

		// 3. 풀에 없으면 새로 생성
		return GLFramebuffer3D::Create(width, height, depth, colorFormats, depthStencilFormat, samples);
	}
}