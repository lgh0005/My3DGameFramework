#include "GraphicsPch.h"
#include "TextureUtils.h"

// TODO : 이후에 MathUtils으로 대체 필요
#include <algorithm>
#include <cmath>

namespace MGF3D
{
	uint32 TextureUtils::MapVkFormatToGLInternal(uint32 vkFormat)
	{
		switch (vkFormat)
		{
		case 131: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		case 133: return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
		case 135: return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
		case 136: return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
		case 137: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		case 139: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
		case 145: return GL_COMPRESSED_RGBA_BPTC_UNORM;
		case 146: return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
		case 37:  return GL_RGBA8;
		case 43:  return GL_SRGB8_ALPHA8;
		case 97:  return GL_RGBA16F;
		default:
			MGF_LOG_WARN
			(
				"TextureUtils: Unknown vkFormat {}. Falling back to GL_RGBA8",
				vkFormat
			);
			return GL_RGBA8;
		}
	}

	uint32 TextureUtils::GetPixelFormatFromInternal(uint32 internalFormat)
	{
		switch (internalFormat)
		{
		case GL_SRGB8_ALPHA8:
		case GL_RGBA8:
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_COMPRESSED_RGBA_BPTC_UNORM:
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			return GL_RGBA;

		case GL_R16F:
		case GL_R32F:
		case GL_RED:
			return GL_RED;

		case GL_RGB8:
		case GL_SRGB8:
		case GL_RGB16F:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
		default:
			return GL_RGB;
		}
	}

	uint32 TextureUtils::GetGLDataTypeFromVk(uint32 vkFormat)
	{
		return CommonUtils::Select(vkFormat == 97, GL_HALF_FLOAT, GL_UNSIGNED_BYTE);
	}

	uint32 TextureUtils::CalculateMaxMipLevels(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return 0;
		uint32 maxSide = std::max(width, height);
		return static_cast<uint32>(std::floor(std::log2(static_cast<float>(maxSide)))) + 1;
	}

	uint32 TextureUtils::CalculateMaxMipLevels(uint32 width, uint32 height, uint32 depth)
	{
		if (width == 0 || height == 0 || depth == 0) return 0;
		uint32 maxSide = std::max({ width, height, depth });
		return static_cast<uint32>(std::floor(std::log2(static_cast<float>(maxSide)))) + 1;
	}
}