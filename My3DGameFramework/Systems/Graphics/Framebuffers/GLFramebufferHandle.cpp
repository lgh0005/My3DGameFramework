#include "GraphicsPch.h"
#include "GLFramebufferHandle.h"

namespace MGF3D
{
	GLFramebufferHandle::GLFramebufferHandle() = default;
	GLFramebufferHandle::~GLFramebufferHandle()
	{ 
		if (m_handle != 0)
		{
			glDeleteFramebuffers(1, &m_handle);
			m_handle = 0;
		}
	}

	void GLFramebufferHandle::Bind(uint32 target) const
	{
		glBindFramebuffer(target, m_handle);
	}

	void GLFramebufferHandle::Unbind(uint32 target) const
	{
		glBindFramebuffer(target, 0);
	}

	void GLFramebufferHandle::Blit
	(
		const GLFramebufferHandle* src, const GLFramebufferHandle* dst, 
		int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1, 
		int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1, 
		uint32 mask, uint32 filter
	)
	{
		uint32 srcHandle = src ? src->GetHandle() : 0;
		uint32 dstHandle = dst ? dst->GetHandle() : 0;

		// 프레임버퍼 고속 복사
		glBindFramebuffer(GL_READ_FRAMEBUFFER, srcHandle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstHandle);
		glBlitFramebuffer
		(
			srcX0, srcY0, srcX1, srcY1,
			dstX0, dstY0, dstX1, dstY1,
			mask, filter
		);

		// Blit 완료 후 안전하게 기본 프레임버퍼로 복구
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	uint32 GLFramebufferHandle::GetTarget() const
	{
		return m_target;
	}

	uint32 GLFramebufferHandle::GetHandle() const
	{
		return m_handle;
	}

	bool GLFramebufferHandle::CheckStatus(uint32 target) const
	{
		GLenum status = glCheckNamedFramebufferStatus(m_handle, target);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			MGF_LOG_ERROR("Framebuffer Incomplete! Error Code: 0x{:X}", status);
			return false;
		}
		return true;
	}
}