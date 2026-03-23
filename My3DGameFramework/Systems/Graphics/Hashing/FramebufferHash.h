#pragma once
#include "Hashing/IHashFunctor.h"

namespace MGF3D
{
	struct FramebufferHash : public IHashFunctor<FramebufferHash, usize>
	{
	public:
		using IHashFunctor<FramebufferHash, usize>::IHashFunctor;
		static constexpr usize HASH_MAGIC = 0x9e3779b9;
		FramebufferHash& operator=(usize h);

	public:
		FramebufferHash
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat,
			uint32 samples,
			uint32 depth = 1
		);

	private:
		static usize Calculate
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat,
			uint32 samples,
			uint32 depth
		);
		static void Combine(usize& seed, uint32 v);
	};
}

MGF_REGISTER_HASH(FramebufferHash, usize)

#include "Hashing/FramebufferHash.inl"