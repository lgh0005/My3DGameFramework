#include "GraphicsPch.h"
#include "GLBufferHandle.h"

namespace MGF3D
{
	GLBufferHandle::GLBufferHandle() = default;
	GLBufferHandle::~GLBufferHandle()
	{
		if (m_handle != 0)
		{
			glDeleteBuffers(1, &m_handle);
			m_handle = 0;
			m_byteSize = 0;
		}
	}

	uint32 GLBufferHandle::GetHandle() const
	{
		return m_handle;
	}

	usize GLBufferHandle::GetByteSize() const
	{
		return m_byteSize;
	}
}