#include "GraphicsPch.h"
#include "GLBufferHandle.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	MGF_IMPLEMENT_CUSTOM_ALLOCATORS(GLBufferHandle)

	GLBufferHandle::GLBufferHandle() = default;
    GLBufferHandle::~GLBufferHandle()
    {
        if (m_allocation.IsValid())
            MGF_VRAM.Deallocate(m_allocation);
    }

    uint64 GLBufferHandle::GetOffset() const
    {
        return m_allocation.GetOffset();
    }

    uint64 GLBufferHandle::GetByteSize() const
    {
        return m_allocation.GetSize();
    }

    const VRAMAllocation& GLBufferHandle::GetAllocation() const
    {
        return m_allocation;
    }
}