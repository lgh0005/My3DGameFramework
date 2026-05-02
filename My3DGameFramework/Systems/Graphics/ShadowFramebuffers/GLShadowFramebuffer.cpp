#include "GraphicsPch.h"
#include "GLShadowFramebuffer.h"
#include "TextureArrays/GLTextureArrayHandle.h"

namespace MGF3D
{
	GLShadowFramebuffer::GLShadowFramebuffer() = default;
	GLShadowFramebuffer::~GLShadowFramebuffer() = default;

	GLShadowFramebufferUPtr GLShadowFramebuffer::Create(const GLTextureArrayHandlePtr& shadowTexture)
	{
		auto fbo = GLShadowFramebufferUPtr(new GLShadowFramebuffer());
		if (!fbo->Init(shadowTexture)) return nullptr;
		return fbo;
	}

	bool GLShadowFramebuffer::Init(const GLTextureArrayHandlePtr& shadowTexture)
	{
		if (!shadowTexture) return false;
		m_shadowTexture = shadowTexture;

		// 1. 프레임버퍼 생성 (부모의 m_handle 사용)
		glCreateFramebuffers(1, &m_handle);

		// 2. 그림자 맵은 Depth 전용이므로 Color Buffer를 사용하지 않음
		glNamedFramebufferDrawBuffer(m_handle, GL_NONE);
		glNamedFramebufferReadBuffer(m_handle, GL_NONE);

		// 3. 기본적으로 0번 레이어를 Depth 어태치먼트로 부착
		glNamedFramebufferTextureLayer(m_handle, GL_DEPTH_ATTACHMENT, m_shadowTexture->GetHandle(), 0, 0);

		return CheckStatus();
	}

	void GLShadowFramebuffer::SwitchLayer(uint32 layer, int32 mipLevel)
	{
		glNamedFramebufferTextureLayer(m_handle, GL_DEPTH_ATTACHMENT, m_shadowTexture->GetHandle(), mipLevel, layer);
	}
}