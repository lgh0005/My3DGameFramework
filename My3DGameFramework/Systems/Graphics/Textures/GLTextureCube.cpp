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
			glGenTextures(1, &m_handle);
			if (m_handle == 0) return false;
			glBindTexture(m_target, m_handle);

			// 6개 면에 대해 루프를 돌며 메모리 할당 및 데이터 업로드
			for (uint32 i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
					m_size, m_size, 0, format, type, pixels ? pixels[i] : nullptr);
			}
		}
		else
		{
			m_handle = handle;
			glBindTexture(m_target, m_handle);

			if (pixels)
			{
				for (uint32 i = 0; i < 6; ++i)
				{
					if (!pixels[i]) continue;
					glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0,
						m_size, m_size, format, type, pixels[i]);
				}
			}
		}

		// 큐브 맵은 S, T, R 세 축의 래핑이 필요합니다.
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_R, wrapR);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);

		// 큐브 맵 이음새(Seam) 방지 설정
		// TODO : 이건 최초 윈도우 생성 시 같이 설정을 해보는 걸 고려할 수 있음
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 

		// 자식 클래스에서 해시 생성 및 저장
		m_hash = TextureHash(m_target, internalFormat, m_size, m_size);

		glBindTexture(m_target, 0);
		return true;
	}

	void GLTextureCube::Bind(uint32 slot) const
	{
		if (m_handle)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(m_target, m_handle);
		}
	}

	void GLTextureCube::Unbind(uint32 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_target, 0);
	}

	void GLTextureCube::GenerateMipmap()
	{
		if (m_handle == 0) return;

		Bind(0);
		glGenerateMipmap(m_target);
		Unbind(0);
	}
}