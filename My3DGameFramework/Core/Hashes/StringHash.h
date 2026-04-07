#pragma once
#include "Hashes/IHashFunctor.h"
#include "Constants/ConstantStringHash.h"

namespace MGF3D
{
	class StringHash : public IHashFunctor<StringHash>
	{
	public:
		using IHashFunctor<StringHash>::IHashFunctor;
		constexpr StringHash(StringView sv);

	private:
		static constexpr uint32 Calculate(cstr str, usize len);
	};
}

#include "Hashes/StringHash.inl"

MGF_REGISTER_HASH(StringHash, uint64)