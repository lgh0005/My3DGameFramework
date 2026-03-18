#include "GraphicsPch.h"
#include "GLBufferHandle.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
	MGF_IMPLEMENT_CUSTOM_ALLOCATORS(GLBufferHandle)

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