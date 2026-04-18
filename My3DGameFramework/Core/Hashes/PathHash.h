#pragma once
#include "Hashes/IHashFunctor.h"
#include "Constants/ConstantStringHash.h"

namespace MGF3D
{
	class PathHash : public IHashFunctor<PathHash>
	{
	public:
		using IHashFunctor<PathHash>::IHashFunctor;
		constexpr PathHash(StringView sv);

	private:
		static constexpr uint64 Calculate(cstr str, usize len);
	};
}

#include "Hashes/PathHash.inl"

MGF_REGISTER_HASH(PathHash, uint64)