#pragma once
#include "GLTextureArrayHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2DArray)
		class GLTexture2DArray : public GLTextureArrayHandle
	{
	public:
		virtual ~GLTexture2DArray() override;

	public:
		static GLTexture2DArrayUPtr Create
		(
			uint32 width, 
			uint32 height, 
			uint32 layers
		);

		virtual bool OnSyncCreate() override;

	private:
		GLTexture2DArray();
		bool Init
		(
			uint32 width, 
			uint32 height, 
			uint32 layers
		);
	};
}