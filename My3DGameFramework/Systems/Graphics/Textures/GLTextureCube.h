#pragma once
#include "Textures/GLTextureHandle.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)
		class GLTextureCube : public GLTextureHandle
	{
	public:
		GLTextureCube();
		virtual ~GLTextureCube() override;
		uint32 GetSize() const { return m_size; }

	/*======================//
	//   GLTextureCube Type //
	//======================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;
		virtual bool OnSyncCreate() override;

	private:
		bool Init
		(
			int32  size,
			uint32 vkFormat, uint32 levels,
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter
		);
		uint32 m_size{ 0 };
	};
}