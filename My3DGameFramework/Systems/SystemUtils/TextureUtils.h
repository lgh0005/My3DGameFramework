#pragma once

namespace MGF3D
{
	class TextureUtils
	{
		MGF_DECLARE_UTILITIES(TextureUtils)

	public:
		static uint32 MapVkFormatToGLInternal(uint32 vkFormat);
		static uint32 GetPixelFormatFromInternal(uint32 internalFormat);
		static uint32 GetGLDataTypeFromVk(uint32 vkFormat);

	public:
		static uint32 CalculateMaxMipLevels(uint32 width, uint32 height);
		static uint32 CalculateMaxMipLevels(uint32 width, uint32 height, uint32 depth);
	};
}