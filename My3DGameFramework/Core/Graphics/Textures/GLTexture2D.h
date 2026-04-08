#pragma once
#include "Graphics/Textures/GLTextureHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)
	class GLTexture2D : public GLTextureHandle
	{
	public:
		virtual ~GLTexture2D() override;
		static GLTexture2DPtr Create
		(
			uint32 width, uint32 height,
			uint32 vkFormat, const void* pixels,
			uint32 levels = 1,
			uint32 wrapS = GL_REPEAT, uint32 wrapT = GL_REPEAT,
			uint32 minFilter = GL_LINEAR, uint32 magFilter = GL_LINEAR,
			uint32 handle = 0
		);

	public:
		void UpdateSubData
		(
			uint32 offsetX, uint32 offsetY,
			uint32 width, uint32 height,
			uint32 format, uint32 type,
			const void* pixels
		);

		uint32 GetWidth()  const { return m_width; }
		uint32 GetHeight() const { return m_height; }

	private:
		GLTexture2D();
		bool Init
		(
			uint32 width, uint32 height,
			uint32 vkFormat, const void* pixels,
			uint32 levels,
			uint32 wrapS, uint32 wrapT,
			uint32 minFilter, uint32 magFilter,
			uint32 handle
		);

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}