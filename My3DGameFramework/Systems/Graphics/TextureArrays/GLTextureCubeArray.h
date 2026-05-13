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

		virtual bool OnSyncCreate() override;

	private:
		GLTextureCubeArray();
		bool Init
		(
			uint32 width, uint32 height, 
			uint32 lightCount
		);

		uint32 m_lightCount{ 0 };
	};
}