#include "GraphicsPch.h"
#include "GLTextureCube.h"
#include "Utils/TextureUtils.h"

namespace MGF3D
{
	GLTextureCube::GLTextureCube() = default;
	GLTextureCube::~GLTextureCube() = default;

	GLTextureCubePtr GLTextureCube::Create
	(
		int32 size, uint32 vkFormat, const void* pixels[6],
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		auto texture = GLTextureCubePtr(new GLTextureCube());
		if (!texture->Init(size, vkFormat, pixels, wrapS, wrapT, wrapR, minFilter, magFilter, handle))
			return nullptr;
		return texture;
	}

	bool GLTextureCube::Init
	(
		int32 size, uint32 vkFormat, const void* pixels[6],
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		if (size <= 0) return false;

		m_target = GL_TEXTURE_CUBE_MAP;
		m_size = static_cast<uint32>(size);

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format		  = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type			  = TextureUtils::GetGLDataTypeFromVk(vkFormat);

		if (!handle)
		{
			glCreateTextures(m_target, 1, &m_handle);
			if (m_handle == 0) return false;
			glTextureStorage2D(m_handle, 1, internalFormat, m_size, m_size);

			// 6개 면에 대해 루프를 돌며 메모리 할당 및 데이터 업로드
			if (pixels)
			{
				for (uint32 i = 0; i < 6; ++i)
				{
					if (!pixels[i]) continue;
					glTextureSubImage3D(m_handle, 0, 0, 0, i, m_size, m_size, 1, format, type, pixels[i]);
				}
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

		// 자식 클래스에서 해시 생성 및 저장
		m_hash = TextureHash(m_target, internalFormat, m_size, m_size);

		return true;
	}

	void GLTextureCube::Bind(uint32 slot) const
	{
		if (m_handle) glBindTextureUnit(slot, m_handle);
	}

	void GLTextureCube::Unbind(uint32 slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void GLTextureCube::GenerateMipmap()
	{
		if (m_handle == 0) return;
		glGenerateTextureMipmap(m_handle);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
}