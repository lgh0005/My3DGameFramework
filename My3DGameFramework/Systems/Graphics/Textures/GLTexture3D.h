#pragma once
#include "Textures/GLTextureHandle.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture3D)
	class GLTexture3D : public GLTextureHandle
	{
	public:
		GLTexture3D();
		virtual ~GLTexture3D() override;

	/*======================//
	//    GLTexture3D Type  //
	//======================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;
		virtual bool OnSyncCreate() override;

	public:
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
		bool Init
		(
			int32  width, int32 height, int32 depth,
			uint32 vkFormat, uint32 levels,
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter
		);

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_depth{ 0 };
	};
}