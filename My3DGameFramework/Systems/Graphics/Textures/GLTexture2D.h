#pragma once
#include "Textures/GLTexture.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)
	class GLTexture2D : public GLTexture
	{
	public:
		virtual ~GLTexture2D() override;
		static GLTexture2DPtr Create
		(
			int32  width, int32  height,
			uint32 vkFormat, const void* pixels,
			uint32 wrapS, uint32 wrapT,
			uint32 minFilter, uint32 magFilter,
			uint32 handle = 0
		);

	public:
		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;
		void GenerateMipmap();
		void UpdateSubData
		(
			int32 offsetX, int32 offsetY,
			int32 width, int32 height,
			uint32 format, uint32 type,
			const void* pixels
		);

		uint32 GetWidth()  const { return m_width; }
		uint32 GetHeight() const { return m_height; }

	private:
		GLTexture2D();
		bool Init
		(
			int32  width, int32  height,
			uint32 vkFormat, const void* pixels,
			uint32 wrapS, uint32 wrapT,
			uint32 minFilter, uint32 magFilter,
			uint32 handle
		);

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}