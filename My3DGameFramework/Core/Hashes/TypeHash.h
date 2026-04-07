#pragma once
#include "Hashes/IHashFunctor.h"
#include "Constants/ConstantStringHash.h"

namespace MGF3D
{
	class TypeHash : public IHashFunctor<TypeHash>
	{
	public:
		using IHashFunctor<TypeHash>::IHashFunctor;
		constexpr TypeHash(StringView sv);

	private:
		static constexpr uint64 Calculate(cstr str, usize len);
	};
}

#include "Hashes/TypeHash.inl"

MGF_REGISTER_HASH(TypeHash, uint64)