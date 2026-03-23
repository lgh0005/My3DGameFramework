#pragma once
#include "Pools/TexturePool.h"
#include "Textures/GLTexture3D.h"

namespace MGF3D
{
	class Texture3DPool : public TexturePool<GLTexture3D>
	{
		using Super = TexturePool<GLTexture3D>;

	public:
		Texture3DPool(uint32 maxCapacity = 100);
		~Texture3DPool();

	public:
		GLTexture3DPtr Request
		(
			uint32 w, uint32 h, uint32 d,
			uint32 vkFormat,
			uint32 minFilter, uint32 magFilter,
			uint32 wrapS, uint32 wrapT, uint32 wrapR
		);
	};
}