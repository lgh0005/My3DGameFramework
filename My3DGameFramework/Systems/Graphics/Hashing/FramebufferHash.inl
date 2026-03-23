#pragma once

namespace MGF3D
{
	inline FramebufferHash& FramebufferHash::operator=(usize h)
	{
		value = h;
		return *this;
	}

	inline FramebufferHash::FramebufferHash
	(
		uint32 width, uint32 height,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 depth
	)
		: IHashFunctor<FramebufferHash, usize>(Calculate(width, height, colorFormats, depthStencilFormat, samples, depth)) {}

	inline usize FramebufferHash::Calculate
	(
		uint32 width, uint32 height,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 depth
	)
	{
		usize seed = 0;
		Combine(seed, width);
		Combine(seed, height);
		Combine(seed, depth); // 3D 프레임버퍼를 위한 깊이값 해시 조합 추가
		Combine(seed, samples);
		Combine(seed, depthStencilFormat);

		for (uint32 format : colorFormats)
			Combine(seed, format);

		return seed;
	}

	inline void FramebufferHash::Combine(usize& seed, uint32 v)
	{
		// Golden Ratio 기반 해시 조합 (Boost.Hash 스타일)
		seed ^= static_cast<usize>(v) + HASH_MAGIC + (seed << 6) + (seed >> 2);
	}
}