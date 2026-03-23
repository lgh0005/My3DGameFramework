#pragma once
#include "Framebuffers/GLFramebuffer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture3D)

	MGF_CLASS_PTR(GLFramebuffer3D)
	class GLFramebuffer3D : public GLFramebuffer
	{
	public:
		virtual ~GLFramebuffer3D() override;
		static GLFramebuffer3DPtr Create
		(
			uint32 width, uint32 height, uint32 depth,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1,
			uint32 handle = 0
		);
		
	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
		void Resize(uint32 w, uint32 h);
		void Resize(uint32 w, uint32 h, uint32 d);
		void BindSlice(uint32 slice) const;

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
		GLTexture3DPtr GetColorAttachment(uint32 index) const;
		GLTexture3DPtr GetDepthStencilAttachment() const;
		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		uint32 GetDepth() const { return m_depth; }
		bool IsMultiSampled() const { return m_samples > 1; }

	private:
		GLFramebuffer3D();
		bool Init
		(
			uint32 width, uint32 height, uint32 depth,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat,
			uint32 samples,
			uint32 handle
		);

		void RefreshAttachments();
		Recti GetBlitArea(const GLFramebufferPtr& fbo, const Ptr<Recti>& rect) const;


	private:
		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_depth{ 1 };
		uint32 m_samples{ 1 };
		uint32 m_depthStencilFormat{ 0 };
		SVector<uint32> m_colorFormats;

		SVector<GLTexture3DPtr> m_colorTextures;
		GLTexture3DPtr m_depthStencilTexture;

	};
}