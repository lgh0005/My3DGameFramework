#pragma once
#include "Framebuffers/GLFramebuffer.h"
#include "Layouts/FramebufferLayout.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)

	MGF_CLASS_PTR(GLFramebuffer2D)
	class GLFramebuffer2D : public GLFramebuffer
	{
	public:
		virtual ~GLFramebuffer2D() override;
		static GLFramebuffer2DPtr Create(const FramebufferLayout& layout, uint32 handle = 0);

	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Resize(uint32 w, uint32 h) override;

	public:
		virtual void Blit
		(
			const GLFramebufferPtr& src, 
			const GLFramebufferPtr& dst,
			const Ptr<Recti> srcRect = nullptr, 
			const Ptr<Recti> dstRect = nullptr,
			uint32 mask = GL_COLOR_BUFFER_BIT, 
			uint32 filter = GL_NEAREST
		) override;

	public:
		GLTexture2DPtr GetColorAttachment(uint32 index) const;
		GLTexture2DPtr GetDepthStencilAttachment() const;

		uint32 GetWidth() const { return m_layout.width; }
		uint32 GetHeight() const { return m_layout.height; }

	private:
		GLFramebuffer2D();
		bool Init(const FramebufferLayout& layout, uint32 handle);
		void RefreshAttachments();
		Recti GetBlitArea(const GLFramebufferPtr& fbo, const Ptr<Recti>& rect) const;

		FramebufferLayout m_layout;
		SVector<GLTexture2DPtr> m_colorTextures;
		GLTexture2DPtr m_depthStencilTexture;
	};
}