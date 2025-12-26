#include "EnginePch.h"
#include "PostProcessFrameBuffer.h"
#include "Resources/Texture.h"

PostProcessFramebuffer::PostProcessFramebuffer() = default;
PostProcessFramebuffer::~PostProcessFramebuffer() = default;

PostProcessFramebufferUPtr PostProcessFramebuffer::Create(int32 width, int32 height, bool useDepth)
{
	auto fbo = PostProcessFramebufferUPtr(new PostProcessFramebuffer());
	if (!fbo->Init(width, height, useDepth)) return nullptr;
	return std::move(fbo);
}

bool PostProcessFramebuffer::Init(int32 width, int32 height, bool useDepth)
{
	m_width = width;
	m_height = height;

	// 2. FBO 생성
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// 3. 컬러 텍스처 생성 (HDR: RGBA16F)
	// Bloom, ToneMapping 등을 위해 HDR 포맷 필수
	auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	texture->SetFilter(GL_LINEAR, GL_LINEAR);
	texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);
	m_textures.push_back(std::move(texture));

	// 4. Draw Buffer 설정
	uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// 5. 깊이 버퍼 (옵션)
	// SRP 메인 버퍼로 쓸 때는 Depth가 필요하고, Bloom 핑퐁용일 때는 불필요
	if (useDepth)
	{
		glGenRenderbuffers(1, &m_depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	}

	// 6. 완성 확인
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SPDLOG_ERROR("PostProcess Framebuffer Init Failed!");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void PostProcessFramebuffer::AttachTexture(Texture* texture, int32 attachmentIndex)
{
	// 1. 부모 클래스의 공통 멤버 m_fbo 사용
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// 2. 텍스처 교체 (지정한 인덱스 슬롯에 끼우기)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex,
		GL_TEXTURE_2D, texture->Get(), 0);

	// 3. Draw Buffer 갱신
	uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 + (uint32)attachmentIndex };
	glDrawBuffers(1, attachments);
}

