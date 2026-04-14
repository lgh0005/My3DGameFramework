#include "GraphicsPch.h"
#include "GLTextureCube.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTextureCube::GLTextureCube() = default;
	GLTextureCube::~GLTextureCube() = default;

	/*======================//
	//   GLTextureCube Type //
	//======================*/
	int16 GLTextureCube::s_typeIndex = -1;
	const MGFType* GLTextureCube::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool GLTextureCube::OnSyncCreate()
	{
		if (m_ktxTexture == nullptr) return false;

		// 1. KTX 메타데이터 추출 (큐브맵은 baseWidth가 size가 됩니다)
		uint32 size = m_ktxTexture->baseWidth;
		uint32 vkFormat = m_ktxTexture->vkFormat;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;

		// 2. 스토리지 할당 (Init 호출)
		// 큐브맵은 기본적으로 CLAMP_TO_EDGE를 사용하여 심(Seam) 현상을 방지합니다.
		if (!Init(size, vkFormat, levels,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
			GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)) return false;

		// 3. 데이터 업로드 (Level -> Face 순으로 루프)
		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);
		for (uint32 level = 0; level < levels; ++level)
		{
			uint32 levelSize = std::max(1u, size >> level);

			for (uint32 face = 0; face < 6; ++face)
			{
				ktx_size_t offset;
				// KTX에서 특정 레벨, 특정 면의 데이터 오프셋을 가져옵니다.
				KTX_error_code res = ktxTexture_GetImageOffset(ktxTexture(m_ktxTexture), level, 0, face, &offset);
				if (res != KTX_SUCCESS) break;

				void* data = ktxTexture_GetData(ktxTexture(m_ktxTexture)) + offset;
				ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(m_ktxTexture), level);

				// DSA에서는 큐브맵 업로드 시 glTextureSubImage3D를 사용하며, 
				// zoffset 위치(5번째 인자)가 Face 인덱스(0~5)가 됩니다.
				if (isCompressed)
				{
					glCompressedTextureSubImage3D(m_handle, level, 0, 0, face,
						levelSize, levelSize, 1, internalFormat, static_cast<GLsizei>(imageSize), data);
				}
				else
				{
					glTextureSubImage3D(m_handle, level, 0, 0, face,
						levelSize, levelSize, 1, format, type, data);
				}
			}
		}

		// 4. 자원 정리
		ktxTexture_Destroy(ktxTexture(m_ktxTexture));
		m_ktxTexture = nullptr;

		m_state = EResourceState::Ready;
		return true;
	}

	bool GLTextureCube::Init
	(
		int32 size, 
		uint32 vkFormat, uint32 levels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter
	)
	{
		if (size <= 0) return false;

		m_target = GL_TEXTURE_CUBE_MAP;
		m_size = static_cast<uint32>(size);

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 mipmapLevel = CommonUtils::Select
		(
			levels == 0,
			TextureUtils::CalculateMaxMipLevels(m_size, m_size),
			levels
		);

		// 새 텍스처 생성 및 스토리지 할당
		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// 큐브맵 역시 glTextureStorage2D를 사용합니다. 
		// 타겟이 CUBE_MAP이면 내부적으로 6개 면에 대한 메모리를 모두 할당합니다.
		glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_size, m_size);

		// 파라미터 설정 (S, T, R 세 축 모두 설정)
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, wrapR);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, magFilter);

		return true;
	}
}