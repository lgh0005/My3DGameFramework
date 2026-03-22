#pragma once
#include "Textures/GLTexture.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture3D)
	class GLTexture3D : public GLTexture 
	{
	public:
		virtual ~GLTexture3D() override;
		static GLTexture3DPtr Create
		(
			int32  width, int32 height, int32 depth,
			uint32 vkFormat, const void* pixels,
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter,
			uint32 handle = 0
		);

	public:
		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;
		void GenerateMipmap();
		void UpdateSubData
		(
			int32 offsetX, int32 offsetY, int32 offsetZ,
			int32 width, int32 height, int32 depth,
			uint32 format, uint32 type,
			const void* pixels
		);

		uint32 GetWidth()  const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		uint32 GetDepth()  const { return m_depth; }

	private:
		GLTexture3D();
		bool Init
		(
			int32  width, int32 height, int32 depth,
			uint32 vkFormat, const void* pixels,
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter,
			uint32 handle
		);

		uint32 m_width		{ 0 };
		uint32 m_height		{ 0 };
		uint32 m_depth		{ 0 };
	};
}