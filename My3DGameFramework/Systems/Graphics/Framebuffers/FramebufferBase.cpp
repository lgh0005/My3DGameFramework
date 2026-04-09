#include "EnginePch.h"
#include "FramebufferBase.h"

FramebufferBase::FramebufferBase() = default;
FramebufferBase::~FramebufferBase()
{
	Clean();
}

void FramebufferBase::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FramebufferBase::BindToDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferBase::Blit(FramebufferBase* src, FramebufferBase* dst, uint32 mask, uint32 filter)
{
	if (!src || !dst) return;
	int32 srcW = src->GetWidth(); int32 srcH = src->GetHeight();
	int32 dstW = dst->GetWidth(); int32 dstH = dst->GetHeight();
	BlitRegion(src, 0, 0, srcW, srcH, dst, 0, 0, dstW, dstH, mask, filter);
}

void FramebufferBase::BlitRegion(FramebufferBase* src, int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1, FramebufferBase* dst, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1, uint32 mask, uint32 filter)
{
	if (!src || !dst) return;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->Get());
	glBlitFramebuffer
	(
		srcX0, srcY0, srcX1, srcY1,
		dstX0, dstY0, dstX1, dstY1,
		mask, filter
	);
	BindToDefault();
}

bool FramebufferBase::CheckFramebufferStatus() const
{
	Bind();
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("{} Framebuffer Incomplete! Error Code: 0x{:X}", status);
		return false;
	}
	return true;
}

void FramebufferBase::Clean()
{
	if (m_fbo)
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}
}