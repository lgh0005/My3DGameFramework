#pragma once
#include "Hashes/IHashFunctor.h"

namespace MGF3D
{
	struct ObjectIDHash : public IHashFunctor<ObjectIDHash, uint32>
	{
		using Super = IHashFunctor<ObjectIDHash, uint32>;

		// 하위 20비트: 인덱스 (약 100만 개)
		// 상위 12비트: 세대 (4096번 재사용 가능)
		static constexpr uint32 INDEX_MASK = 0xFFFFF;
		static constexpr uint32 GEN_MASK = 0xFFF00000;
		static constexpr uint32 GEN_SHIFT = 20;

		/*================================//
		//    ObjectIDHash Constructors   //
		//================================*/
		constexpr ObjectIDHash();
		constexpr ObjectIDHash(uint32 val);
		constexpr ObjectIDHash(uint32 index, uint32 generation);
		
		/*================================//
		//      ObjectIDHash Helpers      //
		//================================*/
		inline uint32 GetIndex() const;
		inline uint32 GetGeneration() const;
	};
}

#include "Hashes/ObjectIDHash.inl"

MGF_REGISTER_HASH(ObjectIDHash, uint32)