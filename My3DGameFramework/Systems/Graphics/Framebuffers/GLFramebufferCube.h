#pragma once
#include "Framebuffers/GLFramebufferHandle.h"
#include "Textures/GLTextureCube.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLFramebufferCube)
	class GLFramebufferCube : public GLFramebufferHandle
	{
	public:
		virtual ~GLFramebufferCube() override;
		static GLFramebufferCubePtr Create
		(
			const Vector<GLTextureCubePtr>& colorAttachments,
			const GLTextureCubePtr& depthAttachment = nullptr
		);

	public:
		void AttachFace(uint32 attachmentTarget, const GLTextureCubePtr& texture, uint32 faceIndex, int32 mipLevel = 0);
		GLTextureCubePtr GetColorAttachment(uint32 index = 0) const;
		GLTextureCubePtr GetDepthAttachment() const;
		uint32 GetSize() const { return m_size; }

	private:
		GLFramebufferCube();
		bool Init
		(
			const Vector<GLTextureCubePtr>& colorAttachments,
			const GLTextureCubePtr& depthAttachment
		);

		Vector<GLTextureCubePtr> m_colorAttachments;
		GLTextureCubePtr m_depthAttachment;
		uint32 m_size{ 0 };
	};
}