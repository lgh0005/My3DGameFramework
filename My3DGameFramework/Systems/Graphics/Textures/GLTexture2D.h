#pragma once
#include "Textures/GLTextureHandle.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)
	class GLTexture2D : public GLTextureHandle
	{
	public:
		GLTexture2D();
		virtual ~GLTexture2D() override;

	/*======================//
	//   GLTexture2D Type   //
	//======================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;
		virtual bool OnSyncCreate() override;

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
		bool Init
		(
			uint32 width, uint32 height,
			uint32 vkFormat, uint32 levels,
			uint32 wrapS, uint32 wrapT,
			uint32 minFilter, uint32 magFilter
		);

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}