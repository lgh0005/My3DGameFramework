#pragma once
#include "Hashing/IHashFunctor.h"
#include "Layouts/FramebufferLayout.h"

namespace MGF3D
{
	struct FramebufferHash : public IHashFunctor<FramebufferHash, usize>
	{
	public:
		using IHashFunctor<FramebufferHash, usize>::IHashFunctor;
		static constexpr usize HASH_MAGIC = 0x9e3779b9;
		FramebufferHash& operator=(usize h);

	public:
		FramebufferHash(const FramebufferLayout& desc);

	private:
		static usize Calculate(const FramebufferLayout& desc);
		static void Combine(usize& seed, uint32 v);
	};
}

MGF_REGISTER_HASH(FramebufferHash, usize)

#include "Hashing/FramebufferHash.inl"