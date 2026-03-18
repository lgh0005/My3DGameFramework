#include "GraphicsPch.h"
#include "VRAMPool.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	VRAMPool::VRAMPool(uint64 poolSize)
		: m_poolSize(poolSize), m_freeList(poolSize), m_mappedPtr(nullptr)
	{
		// Persistent Mapping을 위한 플래그 설정
		// 1. GL_MAP_WRITE_BIT: CPU에서 이 버퍼에 데이터를 쓸 수 있게 합니다.
		// 2. GL_MAP_PERSISTENT_BIT: 버퍼가 GPU에 의해 사용 중일 때도 CPU 포인터를 유지합니다.
		// 3. GL_MAP_COHERENT_BIT: CPU가 쓴 데이터가 GPU 캐시에 자동으로 동기화되도록 보장합니다.
		GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		glCreateBuffers(1, &m_bufferHandle);

		// 1. 물리적인 VRAM 영역을 할당하고, 영구 매핑을 허용하도록 설정
		glNamedBufferStorage(m_bufferHandle, m_poolSize, nullptr, flags);

		// 영구적으로 사용할 수 있는 CPU 측 포인터 획득
		m_mappedPtr = glMapNamedBufferRange(m_bufferHandle, 0, m_poolSize, flags);
		MGF_ASSERT(m_mappedPtr != nullptr, "Failed to map VRAMPool buffer!");
	}

	VRAMPool::~VRAMPool()
	{
		if (m_bufferHandle != 0)
		{
			// 프로그램 종료 시 또는 풀 파괴 시에만 매핑을 해제
			glUnmapNamedBuffer(m_bufferHandle);
			glDeleteBuffers(1, &m_bufferHandle);

			m_bufferHandle = 0;
			m_mappedPtr = nullptr;

			MGF_LOG_INFO("VRAMPool destroyed.");
		}
	}

	uint64 VRAMPool::Allocate(uint64 size, uint32 alignment)
	{
		return m_freeList.Allocate(size, alignment);
	}

	void VRAMPool::Deallocate(uint64 offset)
	{
		m_freeList.Deallocate(offset);
	}

	void VRAMPool::UploadData(uint64 offset, const void* data, uint64 size)
	{
		MGF_ASSERT(m_mappedPtr != nullptr, "VRAMPool is not mapped!");
		MGF_ASSERT(data != nullptr, "UploadData received nullptr data!");
		MGF_ASSERT(offset + size <= m_poolSize, "UploadData bounds exceeded!");

		byte* targetAddress = static_cast<byte*>(m_mappedPtr) + offset;
		MemoryUtils::Memcpy(targetAddress, data, static_cast<usize>(size));
	}

	uint32 VRAMPool::GetBufferHandle() const
	{
		return m_bufferHandle;
	}
}