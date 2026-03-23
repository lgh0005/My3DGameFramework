#pragma once
#include "Framebuffers/GLFramebuffer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)

	MGF_CLASS_PTR(GLFramebuffer2D)
	class GLFramebuffer2D : public GLFramebuffer
	{
	public:
		virtual ~GLFramebuffer2D() override;
		static GLFramebuffer2DPtr Create
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1,
			uint32 handle = 0
		);
		void Resize(uint32 w, uint32 h);

	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;

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
		uint32 GetWidth()  const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		bool IsMultiSampled() const { return m_samples > 1; }

	private:
		GLFramebuffer2D();
		bool Init
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat,
			uint32 samples,
			uint32 handle
		);
		void RefreshAttachments();
		Recti GetBlitArea(const GLFramebufferPtr& fbo, const Ptr<Recti>& rect) const;

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_samples{ 1 };
		uint32 m_depthStencilFormat{ 0 };
		SVector<uint32> m_colorFormats;

		SVector<GLTexture2DPtr> m_colorTextures;
		GLTexture2DPtr m_depthStencilTexture;
	};
}