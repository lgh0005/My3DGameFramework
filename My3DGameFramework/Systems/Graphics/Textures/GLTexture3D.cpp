#include "GraphicsPch.h"
#include "GLTexture3D.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTexture3D::GLTexture3D() = default;
	GLTexture3D::~GLTexture3D() = default;

	/*======================//
	//    GLTexture3D Type  //
	//======================*/
	int16 GLTexture3D::s_typeIndex = -1;
	const MGFType* GLTexture3D::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool GLTexture3D::OnSyncCreate()
	{
		if (m_ktxTexture == nullptr) return false;

		// 1. KTX 메타데이터 추출
		uint32 width = m_ktxTexture->baseWidth;
		uint32 height = m_ktxTexture->baseHeight;
		uint32 depth = m_ktxTexture->baseDepth;
		uint32 vkFormat = m_ktxTexture->vkFormat;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;

		// 2. 스토리지 할당 (Init 호출)
		if (!Init(width, height, depth, vkFormat, levels,
			GL_REPEAT, GL_REPEAT, GL_REPEAT,
			GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)) return false;

		// 3. 데이터 업로드 (3D는 각 레벨이 하나의 볼륨 덩어리입니다)
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);
		for (uint32 level = 0; level < levels; ++level)
		{
			ktx_size_t offset;

			// 3D 텍스처는 face와 layer가 보통 0입니다.
			KTX_error_code res = ktxTexture_GetImageOffset(ktxTexture(m_ktxTexture), level, 0, 0, &offset);
			if (res != KTX_SUCCESS) break;

			uint32 levelWidth = std::max(1u, width >> level);
			uint32 levelHeight = std::max(1u, height >> level);
			uint32 levelDepth = std::max(1u, depth >> level);

			void* data = ktxTexture_GetData(ktxTexture(m_ktxTexture)) + offset;
			ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(m_ktxTexture), level);

			if (isCompressed)
			{
				glCompressedTextureSubImage3D(m_handle, level, 0, 0, 0,
					levelWidth, levelHeight, levelDepth, internalFormat, static_cast<GLsizei>(imageSize), data);
			}
			else
			{
				glTextureSubImage3D(m_handle, level, 0, 0, 0,
					levelWidth, levelHeight, levelDepth, format, type, data);
			}
		}

		// 4. 자원 정리 및 상태 변경
		ktxTexture_Destroy(ktxTexture(m_ktxTexture));
		m_ktxTexture = nullptr;

		m_state = EResourceState::Ready;
		return true;
	}

	bool GLTexture3D::Init
	(
		int32  width, int32 height, int32 depth,
		uint32 vkFormat, uint32 levels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter
	)
	{
		if (width <= 0 || height <= 0 || depth <= 0) return false;

		m_target = GL_TEXTURE_3D;
		m_width = static_cast<uint32>(width);
		m_height = static_cast<uint32>(height);
		m_depth = static_cast<uint32>(depth);

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 mipmapLevel = CommonUtils::Select
		(
			levels == 0,
			TextureUtils::CalculateMaxMipLevels(m_width, m_height, m_depth),
			levels
		);

		// 새 텍스처 생성 (DSA)
		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// Immutable Storage 할당 (3D)
		glTextureStorage3D(m_handle, mipmapLevel, internalFormat, m_width, m_height, m_depth);

		// 파라미터 설정
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, wrapR);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, magFilter);

		return true;
	}

	void GLTexture3D::UpdateSubData
	(
		int32 offsetX, int32 offsetY, int32 offsetZ,
		int32 width, int32 height, int32 depth,
		uint32 format, uint32 type,
		const void* pixels
	)
	{
		if (m_handle == 0 || pixels == nullptr) return;
		glTextureSubImage3D
		(
			m_handle,
			0,
			offsetX, offsetY, offsetZ,
			width, height, depth,
			format,
			type,
			pixels
		);
	}
}