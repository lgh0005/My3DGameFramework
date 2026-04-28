#pragma once

namespace MGF3D
{
	template<typename T, usize ChunkSize>
	inline MGFChunk<T, ChunkSize>::MGFChunk()
	{
		// 1. 객체의 생성자를 호출하지 않고 순수 메모리만 할당
		// 정렬(Alignment)을 보장하기 위해 operator new[]를 사용
		m_buffer = static_cast<T*>(::operator new[](sizeof(T)* ChunkSize));

		// 2. 할당 상태 초기화
		m_allocated.fill(false);
	}

	template<typename T, usize ChunkSize>
	inline MGFChunk<T, ChunkSize>::~MGFChunk()
	{
		// 2. 할당된 객체가 있다면 명시적으로 소멸자를 호출
		for (usize i = 0; i < ChunkSize; ++i)
		{
			if (m_allocated[i])
				m_buffer[i].~T();
		}

		// 3. 할당했던 원시 메모리 해제
		::operator delete[](m_buffer);
	}

	template<typename T, usize ChunkSize>
	template<typename ...Args>
	inline T* MGFChunk<T, ChunkSize>::Allocate(usize index, Args&& ...args)
	{
		// 1. 이미 할당된 인덱스인지 체크
		if (index >= ChunkSize || m_allocated[index])
			return nullptr;

		// 2. Placement New: 준비된 메모리 주소(m_buffer + index)에 객체를 생성
		T* ptr = new (m_buffer + index) T(std::forward<Args>(args)...);

		m_allocated[index] = true;
		return ptr;
	}

	template<typename T, usize ChunkSize>
	inline void MGFChunk<T, ChunkSize>::Free(usize index)
	{
		if (index < ChunkSize && m_allocated[index])
		{
			// 명시적으로 소멸자를 호출하여 자원만 정리
			m_buffer[index].~T();
			m_allocated[index] = false;
		}
	}

	template<typename T, usize ChunkSize>
	inline T* MGFChunk<T, ChunkSize>::GetElement(usize index)
	{
		if (index < ChunkSize && m_allocated[index]) return &m_buffer[index];
		return nullptr;
	}
}