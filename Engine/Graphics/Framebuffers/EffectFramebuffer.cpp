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

void EffectFramebuffer::AttachTextureDirect(int32 index, uint32 textureID)
{
	Bind();

	// 2. 텍스처 교체
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
		GL_TEXTURE_2D, textureID, 0);

	// 3. Draw Buffer 즉시 갱신 (이전 코드와 동일한 로직)
	// "다른 건 모르겠고, 지금 지정한 이 인덱스에만 그려라"
	uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 + (uint32)index };
	glDrawBuffers(1, attachments);
}