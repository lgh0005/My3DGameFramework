#include "GraphicsPch.h"
#include "GLTexture.h"

namespace MGF3D
{
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

	const TextureHash& GLTexture::GetHash() const
	{
		return m_hash;
	}

	uint32 GLTexture::GetHandle() const
	{
		return m_handle;
	}

	uint32 GLTexture::ReleaseHandle()
	{
		uint32 handle = m_handle;
		m_handle = 0;
		m_hash   = 0;
		return handle;
	}
}