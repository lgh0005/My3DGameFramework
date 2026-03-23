#pragma once
#include "Pools/TexturePool.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)

	class Texture2DPool : public TexturePool<GLTexture2D>
	{
		using Super = TexturePool<GLTexture2D>;

	public:
		Texture2DPool(uint32 maxCapacity = 100);
		virtual ~Texture2DPool();

	public:
		GLTexture2DPtr Request
		(
			uint32 w, uint32 h,
			uint32 vkFormat,
			uint32 minFilter, uint32 magFilter,
			uint32 wrapS, uint32 wrapT
		);
	};
}