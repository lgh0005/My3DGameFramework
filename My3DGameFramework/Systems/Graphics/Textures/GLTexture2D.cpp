#include "GraphicsPch.h"
#include "GLTexture2D.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTexture2D::GLTexture2D() = default;
	GLTexture2D::~GLTexture2D() = default;

	/*======================//
	//   GLTexture2D Type   //
	//======================*/
	int16 GLTexture2D::s_typeIndex = -1;
	const MGFType* GLTexture2D::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool GLTexture2D::OnSyncCreate()
	{
		if (m_ktxTexture == nullptr) return false;

		// 1. KTX 메타데이터 추출
		uint32 width = m_ktxTexture->baseWidth;
		uint32 height = m_ktxTexture->baseHeight;
		uint32 vkFormat = m_ktxTexture->vkFormat;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;

		// 2. 스토리지 할당 (Init 호출)
		if (!Init(width, height, vkFormat, levels,
			GL_REPEAT, GL_REPEAT,
			GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)) return false;

		// 3. DSA 기반 데이터 업로드 및 수동 밈맵 생성
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);
		for (uint32 level = 0; level < levels; ++level)
		{
			ktx_size_t offset;
			KTX_error_code res = ktxTexture_GetImageOffset(ktxTexture(m_ktxTexture), level, 0, 0, &offset);
			if (res != KTX_SUCCESS) break;

			uint32 levelWidth = Math::Max(1u, width >> level);
			uint32 levelHeight = Math::Max(1u, height >> level);
			void* data = ktxTexture_GetData(ktxTexture(m_ktxTexture)) + offset;
			ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(m_ktxTexture), level);

			if (isCompressed)
			{
				glCompressedTextureSubImage2D(m_handle, level, 0, 0,
					levelWidth, levelHeight, internalFormat, static_cast<GLsizei>(imageSize), data);
			}
			else
			{
				glTextureSubImage2D(m_handle, level, 0, 0,
					levelWidth, levelHeight, format, type, data);
			}
		}

		// 4. KTX 객체 파괴 및 상태 업데이트
		ktxTexture_Destroy(ktxTexture(m_ktxTexture));
		m_ktxTexture = nullptr;

		m_state = EResourceState::Ready;
		return true;
	}

	bool GLTexture2D::Init
	(
		uint32 width, uint32 height, 
		uint32 vkFormat, uint32 levels,
		uint32 wrapS, uint32 wrapT, 
		uint32 minFilter, uint32 magFilter
	)
	{
		// 0. 텍스쳐 유효성 체크
		if (width <= 0 || height <= 0) return false;

		// 1. 기본 텍스쳐 정보 캐싱
		m_target = GL_TEXTURE_2D;
		m_width = static_cast<uint32>(width);
		m_height = static_cast<uint32>(height);

		// 2. vkFormat을 OpenGL 내부 포맷 정보로 변환
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 mipmapLevel = CommonUtils::Select
		(
			levels == 0,
			TextureUtils::CalculateMaxMipLevels(m_width, m_height),
			levels
		);

		// 새 텍스처 생성 (DSA)
		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// Immutable Storage 할당
		glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_width, m_height);

		// 파라미터 설정
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, magFilter);

		return true;
	}

	void GLTexture2D::UpdateSubData
	(
		uint32 offsetX, uint32 offsetY, 
		uint32 width, uint32 height, 
		uint32 format, uint32 type, 
		const void* pixels
	)
	{
		if (m_handle == 0 || pixels == nullptr) return;
		glTextureSubImage2D(m_handle, 0, offsetX, offsetY, width, height, format, type, pixels);
	}
}