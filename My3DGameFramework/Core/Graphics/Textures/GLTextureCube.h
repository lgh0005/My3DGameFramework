#pragma once
#include "Graphics/Textures/GLTextureHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)
		class GLTextureCube : public GLTextureHandle
	{
	public:
		virtual ~GLTextureCube() override;
		static GLTextureCubePtr Create
		(
			int32  size,		   // 정육면체의 한 면 텍스쳐의 크기
			uint32 vkFormat, 
			const void* pixels[6], // 6개 면에 대한 데이터 (PX, NX, PY, NY, PZ, NZ 순서)
			uint32 levels = 1,
			uint32 wrapS = GL_CLAMP_TO_EDGE, 
			uint32 wrapT = GL_CLAMP_TO_EDGE,
			uint32 wrapR = GL_CLAMP_TO_EDGE,
			uint32 minFilter = GL_LINEAR, 
			uint32 magFilter = GL_LINEAR,
			uint32 handle = 0
		);
		uint32 GetSize() const { return m_size; }

	private:
		GLTextureCube();
		bool Init
		(
			int32  size,
			uint32 vkFormat, const void* pixels[6],
			uint32 levels,
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter,
			uint32 handle
		);
		uint32 m_size{ 0 };
	};
}