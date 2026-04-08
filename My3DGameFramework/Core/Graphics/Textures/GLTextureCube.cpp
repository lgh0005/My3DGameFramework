#include "CorePch.h"
#include "GLTextureCube.h"
#include "CoreUtils/TextureUtils.h"

namespace MGF3D
{
	GLTextureCube::GLTextureCube() = default;
	GLTextureCube::~GLTextureCube() = default;

	GLTextureCubePtr GLTextureCube::Create
	(
		int32 size, 
		uint32 vkFormat, const void* pixels[6],
		uint32 levels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		auto texture = GLTextureCubePtr(new GLTextureCube());
		if (!texture->Init(
			size, 
			vkFormat, pixels, 
			levels, 
			wrapS, wrapT, wrapR, 
			minFilter, magFilter, 
			handle)) return nullptr;
		return texture;
	}

	bool GLTextureCube::Init
	(
		int32 size, 
		uint32 vkFormat, const void* pixels[6],
		uint32 levels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		if (size <= 0) return false;

		m_target = GL_TEXTURE_CUBE_MAP;
		m_size = static_cast<uint32>(size);

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);
		uint32 mipmapLevel = CommonUtils::Select
		(
			levels == 0,
			TextureUtils::CalculateMaxMipLevels(m_size, m_size),
			levels
		);

		if (!handle)
		{
			glCreateTextures(m_target, 1, &m_handle);
			if (m_handle == 0) return false;
			glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_size, m_size);

			// 6개 면에 대해 루프를 돌며 메모리 할당 및 데이터 업로드
			if (pixels)
			{
				for (uint32 i = 0; i < 6; ++i)
				{
					if (!pixels[i]) continue;
					glTextureSubImage3D(m_handle, 0, 0, 0, i, m_size, m_size, 1, format, type, pixels[i]);
				}
				if (mipmapLevel > 1) GenerateMipmap();
			}
		}
		else
		{
			m_handle = handle;
			if (pixels)
			{
				for (uint32 i = 0; i < 6; ++i)
				{
					if (!pixels[i]) continue;
					glTextureSubImage3D(m_handle, 0, 0, 0, i, m_size, m_size, 1, format, type, pixels[i]);
				}
			}
		}

		// 큐브 맵은 S, T, R 세 축의 래핑이 필요합니다.
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, wrapR);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, magFilter);

		return true;
	}
}