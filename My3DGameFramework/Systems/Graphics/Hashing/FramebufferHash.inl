#include "FramebufferHash.h"
#pragma once

namespace MGF3D
{
	inline FramebufferHash& FramebufferHash::operator=(usize h)
	{
		value = h;
		return *this;
	}

	inline MGF3D::FramebufferHash::FramebufferHash(const FramebufferLayout& desc)
		: IHashFunctor<FramebufferHash, usize>(Calculate(desc)) { }

	inline constexpr usize FramebufferHash::Calculate(const FramebufferLayout& desc)
	{
		usize seed = 0;
		Combine(seed, desc.width);
		Combine(seed, desc.height);
		Combine(seed, desc.samples);
		Combine(seed, desc.depthStencilFormat);

		for (uint32 format : desc.colorFormats)
			Combine(seed, format);

		return seed;
	}

	inline constexpr void FramebufferHash::Combine(usize& seed, uint32 v)
	{
		// Golden Ratio 기반 해시 조합 (Boost.Hash 스타일)
		seed ^= static_cast<usize>(v) + HASH_MAGIC + (seed << 6) + (seed >> 2);
	}
}