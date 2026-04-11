#include "GraphicsPch.h"
#include "GLFramebuffer3D.h"

namespace MGF3D
{
	GLFramebuffer3D::GLFramebuffer3D() = default;
	GLFramebuffer3D::~GLFramebuffer3D() = default;

	GLFramebuffer3DPtr GLFramebuffer3D::Create
	(
		const Vector<GLTexture3DPtr>& colorAttachments,
		const GLTexture3DPtr& depthAttachment
	)
	{
		auto fbo = GLFramebuffer3DPtr(new GLFramebuffer3D());
		if (!fbo->Init(colorAttachments, depthAttachment)) return nullptr;
		return fbo;
	}

	bool GLFramebuffer3D::Init
	(
		const Vector<GLTexture3DPtr>& colorAttachments,
		const GLTexture3DPtr& depthAttachment
	)
	{
		if (!colorAttachments.empty())
		{
			m_width = colorAttachments[0]->GetWidth();
			m_height = colorAttachments[0]->GetHeight();
			m_depth = colorAttachments[0]->GetDepth(); // 3D 정보 갱신
		}
		else if (depthAttachment)
		{
			m_width = depthAttachment->GetWidth();
			m_height = depthAttachment->GetHeight();
			m_depth = depthAttachment->GetDepth();
		}
		else
		{
			MGF_LOG_ERROR("GLFramebufferHandle3D init failed: No attachments provided.");
			return false;
		}

		m_colorAttachments = colorAttachments;
		m_depthAttachment = depthAttachment;

		// FBO 객체 즉시 준비
		glCreateFramebuffers(1, &m_handle);

		Vector<uint32> drawBuffers;
		for (uint32 i = 0; i < colorAttachments.size(); ++i)
		{
			uint32 attachment = GL_COLOR_ATTACHMENT0 + i;
			glNamedFramebufferTexture(m_handle, attachment, colorAttachments[i]->GetHandle(), 0);
			drawBuffers.push_back(attachment);
		}

		if (drawBuffers.empty())
		{
			glNamedFramebufferDrawBuffer(m_handle, GL_NONE);
			glNamedFramebufferReadBuffer(m_handle, GL_NONE);
		}
		else
		{
			glNamedFramebufferDrawBuffers(m_handle, static_cast<int32>(drawBuffers.size()), drawBuffers.data());
		}

		if (depthAttachment)
		{
			glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, depthAttachment->GetHandle(), 0);
		}

		return CheckStatus();
	}

	void GLFramebuffer3D::AttachLayer(uint32 attachmentTarget, const GLTexture3DPtr& texture, int32 layer, int32 mipLevel)
	{
		if (!texture) return;
		glNamedFramebufferTextureLayer(m_handle, attachmentTarget, texture->GetHandle(), mipLevel, layer);
	}

	GLTexture3DPtr GLFramebuffer3D::GetColorAttachment(uint32 index) const
	{
		if (index < m_colorAttachments.size()) return m_colorAttachments[index];
		return nullptr;
	}

	GLTexture3DPtr GLFramebuffer3D::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}