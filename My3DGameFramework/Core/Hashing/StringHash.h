#pragma once
#include "Hashing/IHashFunctor.h"

namespace MGF3D
{
	// INFO : FNV-1a 문자열 해싱
	class StringHash : public IHashFunctor<StringHash>
	{
	public:
		// FNV-1a Constants
		static constexpr uint32 OFFSET_BASIS = 2166136261u;
		static constexpr uint32 PRIME = 16777619u;

	public:
		using IHashFunctor<StringHash>::IHashFunctor;
		constexpr StringHash(cstr s);
		StringHash(strview sv);

    private:
		static constexpr uint32 Calculate(cstr str, usize len);
	};
}

MGF_REGISTER_HASH(MGF3D::StringHash)

#include "Hashing/StringHash.inl"