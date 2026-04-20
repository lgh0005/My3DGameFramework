#pragma once
#include "Textures/GLTextureHandle.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)
		class GLTextureCube : public GLTextureHandle
	{
	public:
		virtual ~GLTextureCube() override;
		static GLTextureCubePtr Create(ktxTexture2* ktx);
		static GLTextureCubePtr Create(uint32 size, uint32 vkFormat, uint32 levels = 1);
		uint32 GetSize() const { return m_size; }

	/*======================//
	//   GLTextureCube Type //
	//======================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;
		virtual bool OnSyncCreate() override;

	private:
		GLTextureCube();
		bool CreateFromKtx();
		bool AllocateStorage(uint32 size, uint32 vkFormat, uint32 levels);

		uint32 m_size{ 0 };
		uint32 m_vkFormat{ 0 };
		uint32 m_levels{ 1 };
	};
}