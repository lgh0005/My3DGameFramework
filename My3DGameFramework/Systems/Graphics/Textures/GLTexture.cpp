#include "GraphicsPch.h"
#include "GLTexture.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	MGF_IMPLEMENT_CUSTOM_ALLOCATORS(GLTexture)

	GLTexture::GLTexture() = default;
	GLTexture::~GLTexture()
	{
		if (m_handle)
		{
			glDeleteTextures(1, &m_handle);
			m_handle = 0;
		}
	}

	uint32 GLTexture::GetTarget() const
	{
		return m_target;
	}

	uint32 GLTexture::GetHandle() const
	{
		return m_handle;
	}

	uint32 GLTexture::ReleaseHandle()
	{
		uint32 handle = m_handle;
		m_handle = 0;
		return handle;
	}
}