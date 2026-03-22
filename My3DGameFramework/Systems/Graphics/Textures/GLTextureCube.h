#pragma once
#include "Textures/GLTexture.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)
	class GLTextureCube : public GLTexture
	{
	public:
		virtual ~GLTextureCube() override;
		static GLTextureCubePtr Create
		(
			int32  size, // 정육면체의 한 면 텍스쳐의 크기
			uint32 vkFormat,
			const void* pixels[6], // 6개 면에 대한 데이터 (PX, NX, PY, NY, PZ, NZ 순서)
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter,
			uint32 handle = 0
		);

	public:
		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;
		void GenerateMipmap();
		uint32 GetSize() const { return m_size; }

	private:
		GLTextureCube();
		bool Init
		(
			int32  size,
			uint32 vkFormat,
			const void* pixels[6],
			uint32 wrapS, uint32 wrapT, uint32 wrapR,
			uint32 minFilter, uint32 magFilter,
			uint32 handle
		);

		uint32 m_size	{ 0 };
	};
}