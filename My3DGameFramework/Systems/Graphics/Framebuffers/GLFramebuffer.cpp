#include "GraphicsPch.h"
#include "GLFramebuffer.h"

namespace MGF3D
{
	GLFramebuffer::GLFramebuffer() = default;
	GLFramebuffer::~GLFramebuffer() = default;

	uint32 GLFramebuffer::GetHandle() const
	{
		return m_handle;
	}

	uint32 GLFramebuffer::GetHash() const
	{
		return m_hash;
	}

	uint32 GLFramebuffer::ReleaseHandle()
	{
		uint32 handle = m_handle;
		m_handle = 0;
		m_hash = 0;
		return handle;
	}
}