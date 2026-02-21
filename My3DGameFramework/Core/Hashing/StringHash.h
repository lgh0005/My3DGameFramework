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

	// "대소문자 무시(Case-Insensitive)" 옵션을 StringHash에 어떻게 녹여낼지, 
    // 그리고 이 해시들을 관리할 "StringPool(중복 제거)"이 필요할지 함께 고민해 보면 좋을 것 같아요.
}

MGF_REGISTER_HASH(MGF3D::StringHash)

#include "Hashing/StringHash.inl"