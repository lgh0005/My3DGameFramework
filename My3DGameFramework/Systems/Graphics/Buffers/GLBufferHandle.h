#pragma once
#include "VRAM/VRAMAllocation.h"

namespace MGF3D
{
	class GLBufferHandle
	{
		MGF_DECLARE_CUSTOM_ALLOCATORS;

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