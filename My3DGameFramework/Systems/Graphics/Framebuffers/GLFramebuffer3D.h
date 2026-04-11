#pragma once
#include "Framebuffers/GLFramebufferHandle.h"
#include "Textures/GLTexture3D.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLFramebuffer3D)
	class GLFramebuffer3D : public GLFramebufferHandle
	{
	public:
		virtual ~GLFramebuffer3D() override;
		static GLFramebuffer3DPtr Create
		(
			const Vector<GLTexture3DPtr>& colorAttachments,
			const GLTexture3DPtr& depthAttachment = nullptr
		);

	public:
		void AttachLayer
		(
			uint32 attachmentTarget, const GLTexture3DPtr& texture, 
			int32 layer, int32 mipLevel = 0
		);

		GLTexture3DPtr GetColorAttachment(uint32 index = 0) const;
		GLTexture3DPtr GetDepthAttachment() const;
		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		uint32 GetDepth() const { return m_depth; }

	private:
		GLFramebuffer3D();
		bool Init
		(
			const Vector<GLTexture3DPtr>& colorAttachments,
			const GLTexture3DPtr& depthAttachment
		);

		Vector<GLTexture3DPtr> m_colorAttachments;
		GLTexture3DPtr m_depthAttachment;

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_depth{ 0 };
	};
}