#pragma once
#include "Pools/TexturePool.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureCube)

	class TextureCubePool : public TexturePool<GLTextureCube>
	{
		using Super = TexturePool<GLTextureCube>;

	public:
		TextureCubePool(uint32 maxCapacity = 100);
		~TextureCubePool();

	public:
		GLTextureCubePtr Request
		(
			uint32 size,
			uint32 vkFormat,
			uint32 minFilter, uint32 magFilter,
			uint32 wrapS, uint32 wrapT, uint32 wrapR
		);
	};
}