#include "GraphicsPch.h"
#include "GLTextureArrayHandle.h"

namespace MGF3D
{
	GLTextureArrayHandle::GLTextureArrayHandle() = default;
	GLTextureArrayHandle::~GLTextureArrayHandle()
	{
		if (m_handle != 0)
		{
			glDeleteTextures(1, &m_handle);
			m_handle = 0;
		}
	}

	void GLTextureArrayHandle::Bind(uint32 slot) const
	{
		if (m_handle == 0) return;
		glBindTextureUnit(slot, m_handle);
	}

	void GLTextureArrayHandle::Unbind(uint32 slot)
	{
		glBindTextureUnit(slot, 0);
	}
}