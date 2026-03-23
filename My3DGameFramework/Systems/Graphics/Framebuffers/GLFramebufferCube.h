#pragma once
#include "Framebuffers/GLFramebuffer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)

	MGF_CLASS_PTR(GLFramebufferCube)
	class GLFramebufferCube : public GLFramebuffer
	{
	public:
		virtual ~GLFramebufferCube() override;
		static GLFramebufferCubePtr Create
		(
			uint32 size,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1,
			uint32 handle = 0
		);

	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
		void Resize(uint32 w, uint32 h);
		void Resize(uint32 size);
		void BindFace(uint32 face) const;

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
		GLTextureCubePtr GetColorAttachment(uint32 index) const;
		GLTextureCubePtr GetDepthStencilAttachment() const;
		uint32 GetSize() const { return m_size; }
		bool IsMultiSampled() const { return m_samples > 1; }

	private:
		GLFramebufferCube();
		bool Init
		(
			uint32 size,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat,
			uint32 samples,
			uint32 handle
		);

		void RefreshAttachments();
		Recti GetBlitArea(const GLFramebufferPtr& fbo, const Ptr<Recti>& rect) const;

	private:
		uint32 m_size{ 0 };
		uint32 m_samples{ 1 };
		uint32 m_depthStencilFormat{ 0 };
		SVector<uint32> m_colorFormats;

		SVector<GLTextureCubePtr> m_colorTextures;
		GLTextureCubePtr m_depthStencilTexture;
	};
}