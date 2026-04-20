#pragma once
#include "Textures/GLTextureHandle.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture3D)
	class GLTexture3D : public GLTextureHandle
	{
	public:
		virtual ~GLTexture3D() override;
		static GLTexture3DPtr Create(ktxTexture2* ktx);
		static GLTexture3DPtr Create
		(
			uint32 width, uint32 height, uint32 depth,
			uint32 vkFormat, uint32 levels = 1
		);

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
		GLTexture3D();
		bool CreateFromKtx();
		bool AllocateStorage
		(
			uint32 width, uint32 height, uint32 depth, 
			uint32 vkFormat, uint32 levels
		);

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_depth{ 0 };
		uint32 m_vkFormat{ 0 };
		uint32 m_levels{ 1 };
	};
}