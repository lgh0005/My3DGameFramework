#pragma once
#include "Pointer/PoolAlloc.h"
#include "VRAMFreeList.h"

namespace MGF3D
{
	class VRAMPool : public PoolAlloc
	{
	public:
		VRAMPool(uint64 poolSize);
		virtual ~VRAMPool() override;

	public:
		uint64 Allocate(uint64 size, uint32 alignment);
		void UploadData(uint64 offset, const void* data, uint64 size);
		void Deallocate(uint64 offset);
		uint32 GetBufferHandle() const;

	public:
		uint64 GetUsedMemory()  const { return m_freeList.GetUsedMemory(); }
		uint64 GetTotalMemory() const { return m_poolSize; }

	private:
		void*			m_mappedPtr{ nullptr };

	private:
		uint32          m_bufferHandle	{ 0 };
		uint64          m_poolSize		{ 0 };
		VRAMFreeList    m_freeList;
	};
}