#pragma once
#include "Pointer/PoolAlloc.h"
#include "VRAM/VRAMAllocation.h"

namespace MGF3D
{
	class GLBufferHandle : public PoolAlloc
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		uint64 GetOffset() const;
		uint64 GetByteSize() const;
		const VRAMAllocation& GetAllocation() const;

	protected:
		GLBufferHandle();
		virtual ~GLBufferHandle();

	protected:
		VRAMAllocation m_allocation;
	};
}