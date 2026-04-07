#pragma once
#include "Hashes/TypeHash.h"
#include "Constants/ConstantType.h"

namespace MGF3D
{
	struct MGFType
	{
		// 1. 정체성 (Identity)
		TypeHash    id{ 0 };
		cstr        typeName { nullptr };

		// 2. 관계 인덱스 (Tree Structure in Flat Array)
		int16 selfIndex        { -1 };  // 배열 내 자신의 인덱스
		int16 parentIndex      { -1 };  // 부모 노드의 인덱스
		int16 firstChildIndex  { -1 };  // 첫 번째 자식의 인덱스
		int16 nextSiblingIndex { -1 };  // 바로 다음 형제의 인덱스

		// 3. 족보 체인 (O(1) Inheritance Check)
		Array<TypeHash, MAX_TYPE_DEPTH> chain { };
		uint32   depth{ 0 };

		bool IsA(const MGFType& other) const;
		bool IsValid() const;
	};
}