#include "GraphicsPch.h"
#include "GLBufferHandle.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
	void* GLBufferHandle::operator new(usize size)
	{
		return MGF_MEMORY.Allocate(size);
	}

	void GLBufferHandle::operator delete(void* ptr, usize size)
	{
		MGF_MEMORY.Deallocate(ptr, size);
	}

	GLBufferHandle::GLBufferHandle()
	{
		glCreateBuffers(1, &m_buffer);
	}

	GLBufferHandle::~GLBufferHandle()
	{
		if (m_buffer) 
			glDeleteBuffers(1, &m_buffer);
	}
}