#pragma once
#include "Math/Math.h"

namespace MGF3D
{
	// INFO : 쉽게 말해서 우리가 구현한 메모리 풀에 메모리를 할당할 때는
	// MIN_ALIGNMENT의 배수 크기로 할당이 되어야 하는데 37, 52 바이트와 같은 크기의
	// 메모리 할당을 할 때 이들은 어쩔 수 없이 패딩 값을 둬서 가장 근접한 MIN_ALIGNMENT의
	// 배수만큼의 메모리를 할당하는 수식이라고 보면 됨.
	// 예) 37 -> AlignUp(37, 16) : 48 (낭비 공간 : 11바이트)
	constexpr uintptr MemoryUtils::AlignUp(uintptr address, usize alignment) noexcept
	{
		return (address + (static_cast<uintptr>(alignment) - 1)) & ~(static_cast<uintptr>(alignment) - 1);
	}

	constexpr usize MemoryUtils::GetAlignment(usize alignment) noexcept
	{
		return Math::Max(alignment, MIN_ALIGNMENT);
	}
}