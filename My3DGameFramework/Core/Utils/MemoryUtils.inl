#pragma once
#include "Common/Math.h"

namespace MGF3D
{
	// INFO : 쉽게 말해서 우리가 구현한 메모리 풀에 메모리를 할당할 때는
	// MIN_ALIGNMENT의 배수 크기로 할당이 되어야 하는데 37, 52 바이트와 같은 크기의
	// 메모리 할당을 할 때 이들은 어쩔 수 없이 패딩 값을 둬서 가장 근접한 MIN_ALIGNMENT의
	// 배수만큼의 메모리를 할당하는 수식이라고 보면 됨.
	// 예) 37 -> AlignUp(37, 16) : 48 (낭비 공간 : 11바이트)
	inline constexpr uintptr MemoryUtils::AlignUp(uintptr address, usize alignment) noexcept
	{
		return (address + (static_cast<uintptr>(alignment) - 1)) & ~(static_cast<uintptr>(alignment) - 1);
	}

	inline constexpr usize MemoryUtils::GetAlignment(usize alignment) noexcept
	{
		return Math::Max(alignment, DefaultAlignment);
	}

	inline constexpr uint32 MemoryUtils::MostSignificantBit64(uint64 value) noexcept
	{
		if (value == 0) return 0;

		uint32 result = 0;

		/**
		 * @brief 이진 탐색 기법을 이용한 순수 비트 연산 MSB 찾기
		 * @details O(log N) 복잡도로, 64비트 기준 항상 6번의 조건 검사 내에 완료됩니다.
		*/

		// 1단계: 상위 32비트 확인
		if (value & 0xFFFFFFFF00000000ULL) { value >>= 32; result += 32; }
		// 2단계: 남은 것 중 상위 16비트 확인
		if (value & 0xFFFF0000U) { value >>= 16; result += 16; }
		// 3단계: 상위 8비트 확인
		if (value & 0xFF00U) { value >>= 8;  result += 8; }
		// 4단계: 상위 4비트 확인
		if (value & 0xF0U) { value >>= 4;  result += 4; }
		// 5단계: 상위 2비트 확인
		if (value & 0xCU) { value >>= 2;  result += 2; }
		// 6단계: 마지막 2비트 중 상위 1비트 확인
		if (value & 0x2U) { result += 1; }

		return result;
	}
}