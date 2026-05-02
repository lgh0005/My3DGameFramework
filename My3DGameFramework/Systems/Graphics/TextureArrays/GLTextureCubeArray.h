#pragma once
#include "GLTextureArrayHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCubeArray)
		class GLTextureCubeArray : public GLTextureArrayHandle
	{
	public:
		virtual ~GLTextureCubeArray() override;

		static GLTextureCubeArrayUPtr Create
		(
			uint32 width, uint32 height, 
			uint32 lightCount
		);

	private:
		GLTextureCubeArray();
		bool Init
		(
			uint32 width, uint32 height, 
			uint32 lightCount
		);
	};
}