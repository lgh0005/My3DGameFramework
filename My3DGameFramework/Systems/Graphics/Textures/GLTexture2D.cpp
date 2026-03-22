#include "GraphicsPch.h"
#include "GLTexture2D.h"
#include "Utils/TextureUtils.h"
#include "Hashing/TextureHash.h"

namespace MGF3D
{
	GLTexture2D::GLTexture2D() = default;
	GLTexture2D::~GLTexture2D() = default;

	GLTexture2DPtr GLTexture2D::Create
	(
		int32  width, int32  height,
		uint32 vkFormat, const void* pixels,
		uint32 wrapS, uint32 wrapT,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		auto texture = GLTexture2DPtr(new GLTexture2D());
		if (!texture->Init
		(
			width, height, 
			vkFormat, pixels, 
			wrapS, wrapT, 
			minFilter, magFilter, 
			handle)) return nullptr;
		return texture;
	}

	bool GLTexture2D::Init
	(
		int32  width, int32 height,
		uint32 vkFormat, const void* pixels,
		uint32 wrapS, uint32 wrapT,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		// 0. 텍스쳐 유효성 체크
		if (width <= 0 || height <= 0) return false;

		// 1. 기본 텍스쳐 정보 캐싱
		m_target = GL_TEXTURE_2D;
		m_width = static_cast<uint32>(width);
		m_height = static_cast<uint32>(height);

		// 1. TextureUtils를 통해 vkFormat을 OpenGL 내부 포맷 정보로 변환
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format		  = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type			  = TextureUtils::GetGLDataTypeFromVk(vkFormat);

		// 2. 통합된 분기 처리
		if (!handle)
		{
			// 2-1. 새 텍스처 생성 및 메모리 할당
			glCreateTextures(m_target, 1, &m_handle);
			if (m_handle == 0) return false;
			glTextureStorage2D(m_handle, 1, internalFormat, m_width, m_height);
			if (pixels)
				glTextureSubImage2D(m_handle, 0, 0, 0, m_width, m_height, format, type, pixels);
		}
		else
		{
			// 2-2. 기존 핸들 재사용 및 데이터 덮어쓰기
			m_handle = handle;
			if (pixels)
				glTextureSubImage2D(m_handle, 0, 0, 0, m_width, m_height, format, type, pixels);
		}

		// 3. 파라미터 설정
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, magFilter);

		// 4. 자식 클래스에서 해시 생성 및 저장
		m_hash = TextureHash(m_target, internalFormat, m_width, m_height);

		return true;
	}

	void GLTexture2D::Bind(uint32 slot) const
	{
		if (m_handle) glBindTextureUnit(slot, m_handle);
	}

	void GLTexture2D::Unbind(uint32 slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void GLTexture2D::GenerateMipmap()
	{
		if (m_handle == 0) return;
		glGenerateTextureMipmap(m_handle);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	void GLTexture2D::UpdateSubData
	(
		int32 offsetX, int32 offsetY, 
		int32 width, int32 height, 
		uint32 format, uint32 type, const void* pixels
	)
	{
		if (m_handle == 0 || pixels == nullptr) return;
		glTextureSubImage2D(m_handle, 0, offsetX, offsetY, width, height, format, type, pixels);
	}
}