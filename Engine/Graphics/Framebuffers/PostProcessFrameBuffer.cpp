#include "EnginePch.h"
#include "PostProcessFrameBuffer.h"
#include "Resources/Textures/Texture.h"

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

	// 1. 컬러 및 깊이 텍스쳐 생성 (HDR 대응: RGBA16F)
	CreateAndAttachColor(0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR);
	CreateAndAttachDepth(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);

	// 2. 상태 확인 및 언바인드 (부모의 CheckFramebufferStatus 활용)
	return CheckFramebufferStatus("PostProcess");
}
