#include "CorePch.h"
#include "MGFType.h"

namespace MGF3D
{
	bool MGFType::IsA(const MGFType& other) const
	{
		// 1. 검사하려는 타입이 나보다 깊은 곳에 있다면 절대 조상일 수 없음
		if (other.depth > depth) return false;

		// 2. 내 족보(chain)에서 'other'의 깊이에 해당하는 위치의 ID를 꺼내 비교
		return chain[other.depth] == other.id;
	}

	bool MGFType::IsValid() const
	{
		return id != 0;
	}
}