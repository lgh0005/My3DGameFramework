#pragma once
#include "Hashing/IHashFunctor.h"

namespace MGF3D
{
	struct TextureHash : public IHashFunctor<TextureHash, usize>
	{
	public:
		using IHashFunctor<TextureHash, usize>::IHashFunctor;
		static constexpr usize HASH_MAGIC = 0x9e3779b9;
		TextureHash& operator=(usize h);

	public:
		constexpr TextureHash
		(
			uint32 target, uint32 format, 
			uint32 w, uint32 h = 1, uint32 d = 1
		);

	private:
		static constexpr usize Calculate(uint32 t, uint32 f, uint32 w, uint32 h, uint32 d);
		static constexpr void Combine(usize& seed, uint32 v);
	};
}

MGF_REGISTER_HASH(TextureHash, usize)

#include "Hashing/TextureHash.inl"