#pragma once

namespace MGF3D
{
	/**
	 * @brief Jolt의 RefCounted 객체를 위한 생성 팩토리
	 * 내부적으로 new를 쓰지만, Jolt 할당자 후킹 덕분에 엔진 풀에서 할당됩니다.
	 */
	template<typename T, typename ...Args>
	Ref<T> MakeRef(Args && ...args)
	{
		// 1. 여기서 호출되는 new는 JoltMemoryManager에서 후킹한 JoltAllocator를 타게 됩니다.
		// 2. Ref<T>로 감싸서 반환하므로 소유권 관리도 안전합니다.
		return new T(std::forward<Args>(args)...);
	}
}