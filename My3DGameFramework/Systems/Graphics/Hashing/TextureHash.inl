#include "TextureHash.h"
#pragma once

namespace MGF3D
{
	inline TextureHash& TextureHash::operator=(usize h)
	{
		value = h;
		return *this;
	}

	inline constexpr TextureHash::TextureHash(uint32 target, uint32 format, uint32 w, uint32 h, uint32 d)
		: IHashFunctor<TextureHash, usize>(Calculate(target, format, w, h, d)) { }

	inline constexpr void TextureHash::Combine(usize& seed, uint32 v)
	{
		seed ^= static_cast<usize>(v) + HASH_MAGIC + (seed << 6) + (seed >> 2);
	}

	inline constexpr usize TextureHash::Calculate(uint32 t, uint32 f, uint32 w, uint32 h, uint32 d)
	{
		usize seed = 0;
		Combine(seed, t);
		Combine(seed, f);
		Combine(seed, w);
		Combine(seed, h);
		Combine(seed, d);
		return seed;
	}
}