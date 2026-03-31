#pragma once
#include "Hashing/IHashFunctor.h"

namespace MGF3D
{
	struct InstanceIDHash : public IHashFunctor<InstanceIDHash, uint64>
	{
		constexpr InstanceIDHash(uint32 index, uint32 generation);

		using IHashFunctor<InstanceIDHash, uint64>::IHashFunctor;
		constexpr uint32 GetIndex() const;
		constexpr uint32 GetGeneration() const;
	};
}

MGF_REGISTER_HASH(InstanceIDHash, uint64)

#include "Hashing/InstanceIDHash.inl"