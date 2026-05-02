#pragma once
#include "Framebuffers/GLFramebufferHandle.h"
#include "TextureArrays/GLTextureArrayHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureArrayHandle)

	MGF_CLASS_PTR(GLShadowFramebuffer)
	class GLShadowFramebuffer : public GLFramebufferHandle
	{
	public:
		virtual ~GLShadowFramebuffer() override;
		static GLShadowFramebufferUPtr Create(const GLTextureArrayHandlePtr& shadowTexture);

	public:
		void SwitchLayer(uint32 layer, int32 mipLevel = 0);

	private:
		GLShadowFramebuffer();
		bool Init(const GLTextureArrayHandlePtr& shadowTexture);

		GLTextureArrayHandlePtr m_shadowTexture;
	};
}