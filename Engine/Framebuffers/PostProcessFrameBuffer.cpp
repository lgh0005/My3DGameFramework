#include "EnginePch.h"
#include "PostProcessFrameBuffer.h"
#include "Resources/Texture.h"

DECLARE_DEFAULTS_IMPL(PostProcessFramebuffer)

PostProcessFramebufferUPtr PostProcessFramebuffer::Create(int32 width, int32 height)
{
	auto fbo = PostProcessFramebufferUPtr(new PostProcessFramebuffer());
	if (!fbo->Init(width, height)) return nullptr;
	return std::move(fbo);
}

bool PostProcessFramebuffer::Init(int32 width, int32 height)
{
	m_width = width;
	m_height = height;

	// 2. FBO 생성
	glGenFramebuffers(1, &m_fbo);

	return CreateAttachments();
}

bool PostProcessFramebuffer::CreateAttachments()
{
	ClearAttachments();
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// 1. 컬러 텍스처 생성 (HDR 대응: RGBA16F)
	CreateAndAttachColor(0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR);

	// 2. Draw Buffer 설정 (컬러가 1개뿐이므로 0번 활성화)
	uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// 3. 상태 확인 및 언바인드 (부모의 CheckFramebufferStatus 활용)
	return CheckFramebufferStatus("PostProcess");
}
