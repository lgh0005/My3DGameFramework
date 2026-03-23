#include "GraphicsPch.h"
#include "Framebuffer2DPool.h"
#include "Hashing/FramebufferHash.h"

namespace MGF3D
{
	Framebuffer2DPool::Framebuffer2DPool(uint32 maxCapacity) : Super(maxCapacity) { }
	Framebuffer2DPool::~Framebuffer2DPool() = default;

	GLFramebuffer2DPtr Framebuffer2DPool::Request
	(
		uint32 width, uint32 height,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples
	)
	{
		// 1. 요청 규격으로 해시 생성
		usize hash = FramebufferHash(width, height, colorFormats, depthStencilFormat, samples, 1).value;

		// 2. 부모 클래스의 공통 로직을 사용하여 가용 프레임버퍼 검색
		auto fbo = FindAvailable(hash);
		if (fbo != nullptr) return fbo;

		// 3. 풀에 없으면 새로 생성하여 반환
		return GLFramebuffer2D::Create(width, height, colorFormats, depthStencilFormat, samples);
	}
}