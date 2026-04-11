#include "GraphicsPch.h"
#include "GLFramebuffer2D.h"

namespace MGF3D
{
	GLFramebuffer2D::GLFramebuffer2D() = default;
	GLFramebuffer2D::~GLFramebuffer2D() = default;

	GLFramebuffer2DPtr GLFramebuffer2D::Create
	(
		const Vector<GLTexture2DPtr>& colorAttachments,
		const GLTexture2DPtr& depthAttachment
	)
	{
		auto fbo = GLFramebuffer2DPtr(new GLFramebuffer2D());
		if (!fbo->Init(colorAttachments, depthAttachment)) return nullptr;
		return fbo;
	}

	GLTexture2DPtr GLFramebuffer2D::GetColorAttachment(uint32 index) const
	{
		if (index < m_colorAttachments.size()) return m_colorAttachments[index];
		return nullptr;
	}

	GLTexture2DPtr GLFramebuffer2D::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}

	bool GLFramebuffer2D::Init
	(
		const Vector<GLTexture2DPtr>& colorAttachments,
		const GLTexture2DPtr& depthAttachment
	)
	{
		// 1. 프레임버퍼의 크기는 텍스처의 크기를 따라갑니다.
		if (!colorAttachments.empty())
		{
			m_width = colorAttachments[0]->GetWidth();
			m_height = colorAttachments[0]->GetHeight();
		}
		else if (depthAttachment)
		{
			m_width = depthAttachment->GetWidth();
			m_height = depthAttachment->GetHeight();
		}
		else
		{
			MGF_LOG_ERROR("Framebuffer initialization failed: No attachments provided.");
			return false;
		}

		m_colorAttachments = colorAttachments;
		m_depthAttachment = depthAttachment;

		// 2. FBO 생성
		glCreateFramebuffers(1, &m_handle);

		Vector<uint32> drawBuffers;

		// 3. 컬러 텍스처 장착
		for (uint32 i = 0; i < colorAttachments.size(); ++i)
		{
			uint32 attachment = GL_COLOR_ATTACHMENT0 + i;
			glNamedFramebufferTexture(m_handle, attachment, colorAttachments[i]->GetHandle(), 0);
			drawBuffers.push_back(attachment);
		}

		// 4. 드로우 버퍼(어느 컬러 슬롯들에 그림을 그릴지) 세팅
		if (drawBuffers.empty())
		{
			glNamedFramebufferDrawBuffer(m_handle, GL_NONE);
			glNamedFramebufferReadBuffer(m_handle, GL_NONE);
		}
		else
		{
			// 바인딩 없이 드로우 버퍼 지정
			glNamedFramebufferDrawBuffers(m_handle, static_cast<int32>(drawBuffers.size()), drawBuffers.data());
		}

		// 5. 깊이/스텐실 텍스처 장착
		if (depthAttachment)
			glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, depthAttachment->GetHandle(), 0);

		// 6. 기반 클래스의 상태 체크 호출
		return CheckStatus();
	}
}