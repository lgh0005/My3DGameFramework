#include "GraphicsPch.h"
#include "Pools/FramebufferCubePool.h"
#include "Hashing/FramebufferHash.h"

namespace MGF3D
{
	FramebufferCubePool::FramebufferCubePool(uint32 maxCapacity) : Super(maxCapacity) { }
	FramebufferCubePool::~FramebufferCubePool() = default;

	GLFramebufferCubePtr FramebufferCubePool::Request
	(
		uint32 size,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples
	)
	{
		// 1. 큐브맵 규격에 맞추어 width/height 자리에 size를, depth 자리에 6을 넣어 해시 생성
		usize hash = FramebufferHash(size, size, colorFormats, depthStencilFormat, samples, 6).value;

		// 2. 가용 리스트 검색
		auto fbo = FindAvailable(hash);
		if (fbo != nullptr) return fbo;

		// 3. 풀에 없으면 새로 생성
		return GLFramebufferCube::Create(size, colorFormats, depthStencilFormat, samples);
	}
}