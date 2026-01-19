#include "EnginePch.h"
#include "EffectFramebuffer.h"

DECLARE_DEFAULTS_IMPL(EffectFramebuffer)

EffectFramebufferUPtr EffectFramebuffer::Create(int32 width, int32 height)
{
	auto bloom = EffectFramebufferUPtr(new EffectFramebuffer());
	bloom->Init(width, height);
	return std::move(bloom);
}

EffectFramebufferUPtr EffectFramebuffer::CreateEmpty()
{
	auto fbo = EffectFramebufferUPtr(new EffectFramebuffer());
	if (!fbo->InitEmpty()) return nullptr;
	return std::move(fbo);
}

bool EffectFramebuffer::CreateAttachments()
{
	if (m_isEmpty) return true;

	ClearAttachments();
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	CreateAndAttachColor(0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR);
	return CheckFramebufferStatus("EffectFramebuffer");
}

bool EffectFramebuffer::Init(int32 width, int32 height)
{
	m_width = width;
	m_height = height;
	m_isEmpty = false;
	glGenFramebuffers(1, &m_fbo);
	return CreateAttachments();
}

bool EffectFramebuffer::InitEmpty()
{
	m_width = 1;
	m_height = 1;
	m_isEmpty = true;
	glGenFramebuffers(1, &m_fbo);
	return true;
}
