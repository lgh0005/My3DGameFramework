#include "GraphicsPch.h"
#include "GLFramebufferCube.h"

namespace MGF3D
{
	GLFramebufferCube::GLFramebufferCube() = default;
	GLFramebufferCube::~GLFramebufferCube() = default;

	GLFramebufferCubePtr GLFramebufferCube::Create
	(
		const Vector<GLTextureCubePtr>& colorAttachments,
		const GLTextureCubePtr& depthAttachment
	)
	{
		auto fbo = GLFramebufferCubePtr(new GLFramebufferCube());
		if (!fbo->Init(colorAttachments, depthAttachment)) return nullptr;
		return fbo;
	}

	bool GLFramebufferCube::Init
	(
		const Vector<GLTextureCubePtr>& colorAttachments,
		const GLTextureCubePtr& depthAttachment
	)
	{
		// 텍스처에서 width/height 대신 size를 바로 가져옵니다.
		if (!colorAttachments.empty())
		{
			m_size = colorAttachments[0]->GetSize();
		}
		else if (depthAttachment)
		{
			m_size = depthAttachment->GetSize();
		}
		else
		{
			MGF_LOG_ERROR("GLFramebufferCube init failed: No attachments provided.");
			return false;
		}

		m_colorAttachments = colorAttachments;
		m_depthAttachment = depthAttachment;

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

	void GLFramebufferCube::AttachFace(uint32 attachmentTarget, const GLTextureCubePtr& texture, uint32 faceIndex, int32 mipLevel)
	{
		if (!texture) return;
		glNamedFramebufferTextureLayer(m_handle, attachmentTarget, texture->GetHandle(), mipLevel, faceIndex);
	}

	GLTextureCubePtr GLFramebufferCube::GetColorAttachment(uint32 index) const
	{
		if (index < m_colorAttachments.size()) return m_colorAttachments[index];
		return nullptr;
	}

	GLTextureCubePtr GLFramebufferCube::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}