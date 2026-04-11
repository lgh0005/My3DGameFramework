#pragma once
#include "Framebuffers/GLFramebufferHandle.h"
#include "Textures/GLTexture2D.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLFramebuffer2D)
	class GLFramebuffer2D : public GLFramebufferHandle
	{
	public:
		virtual ~GLFramebuffer2D() override;
		static GLFramebuffer2DPtr Create
		(
			const Vector<GLTexture2DPtr>& colorAttachments,
			const GLTexture2DPtr& depthAttachment = nullptr
		);

	public:
		GLTexture2DPtr GetColorAttachment(uint32 index = 0) const;
		GLTexture2DPtr GetDepthAttachment() const;
		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }

	private:
		GLFramebuffer2D();
		bool Init
		(
			const Vector<GLTexture2DPtr>& colorAttachments,
			const GLTexture2DPtr& depthAttachment
		);

		Vector<GLTexture2DPtr> m_colorAttachments;
		GLTexture2DPtr m_depthAttachment;
		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}