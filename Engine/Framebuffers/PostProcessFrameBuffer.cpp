#include "EnginePch.h"
#include "PostProcessFrameBuffer.h"
#include "Resources/Texture.h"

DECLARE_DEFAULTS_IMPL(PostProcessFramebuffer)

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
		m_depthTexture = Texture::Create(width, height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
		m_depthTexture->SetFilter(GL_NEAREST, GL_NEAREST);
		m_depthTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->Get(), 0);
	}

	// 6. 완성 확인
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("PostProcess Framebuffer Init Failed!");
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

void PostProcessFramebuffer::Blit(PostProcessFramebuffer* src, PostProcessFramebuffer* dst, int32 width, int32 height, uint32 mask, uint32 filter)
{
	// 1. ID 추출 (nullptr이면 0 = 기본 화면)
	// 상속 구조에 따라 GetFBO()나 m_fbo를 사용하세요. 
	// (같은 클래스이므로 private 멤버 m_fbo에 접근 가능합니다.)
	uint32 srcID = src ? src->m_fbo : 0;
	uint32 dstID = dst ? dst->m_fbo : 0;

	// 2. 바인딩 (읽기 전용 / 쓰기 전용 분리)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstID);

	// 3. 고속 복사 (Blit)
	// (srcX0, srcY0, srcX1, srcY1) -> (dstX0, dstY0, dstX1, dstY1)
	glBlitFramebuffer(0, 0, width, height,  // Source Rect
		0, 0, width, height,  // Dest Rect
		mask,                 // 무엇을 복사할지?
		filter);              // 크기가 다를 때 어떻게 처리할지?

	// 4. 언바인딩 (안전장치)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

