#include "GraphicsPch.h"
#include "GLTexture2D.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTexture2D::GLTexture2D() = default;
	GLTexture2D::~GLTexture2D() = default;

	GLTexture2DPtr GLTexture2D::Create(ktxTexture2* ktx)
	{
		if (ktx == nullptr) return nullptr;
		auto texture = SharedPtr<GLTexture2D>(new GLTexture2D());
		texture->SetKtxTexture(ktx);
		texture->SetState(EResourceState::Loaded);
		return texture;
	}

	GLTexture2DPtr GLTexture2D::Create
	(
		uint32 width, uint32 height, 
		uint32 vkFormat, uint32 levels
	)
	{
		auto texture = SharedPtr<GLTexture2D>(new GLTexture2D());
		texture->m_width = width;
		texture->m_height = height;
		texture->SetState(EResourceState::Loaded);
		return texture;
	}

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
		bool success = CommonUtils::Select
		(
			m_ktxTexture != nullptr,
			CreateFromKtx(),
			AllocateStorage(m_width, m_height, m_vkFormat, m_levels)
		);

		if (success) m_state = EResourceState::Ready;
		return success;
	}

	bool GLTexture2D::CreateFromKtx()
	{
		uint32 width = m_ktxTexture->baseWidth;
		uint32 height = m_ktxTexture->baseHeight;
		uint32 vkFormat = m_ktxTexture->vkFormat;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;

		// KTX 정보를 바탕으로 GPU 스토리지 할당
		if (!AllocateStorage(width, height, vkFormat, levels))
			return false;

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);

		for (uint32 level = 0; level < levels; ++level)
		{
			ktx_size_t offset;
			if (ktxTexture_GetImageOffset(ktxTexture(m_ktxTexture), level, 0, 0, &offset) != KTX_SUCCESS) break;

			uint32 levelWidth = Math::Max(1u, width >> level);
			uint32 levelHeight = Math::Max(1u, height >> level);
			void* data = ktxTexture_GetData(ktxTexture(m_ktxTexture)) + offset;
			ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(m_ktxTexture), level);

			if (isCompressed)
				glCompressedTextureSubImage2D(m_handle, level, 0, 0, levelWidth, levelHeight, internalFormat, static_cast<GLsizei>(imageSize), data);
			else
				glTextureSubImage2D(m_handle, level, 0, 0, levelWidth, levelHeight, format, type, data);
		}

		ktxTexture_Destroy(ktxTexture(m_ktxTexture));
		m_ktxTexture = nullptr;
		return true;
	}

	bool GLTexture2D::AllocateStorage(uint32 width, uint32 height, uint32 vkFormat, uint32 levels)
	{
		if (width <= 0 || height <= 0) return false;

		m_target = GL_TEXTURE_2D;
		m_width = width;
		m_height = height;

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 mipmapLevel = CommonUtils::Select(levels == 0, TextureUtils::CalculateMaxMipLevels(m_width, m_height), levels);

		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// Immutable Storage 할당
		glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_width, m_height);

		// 기본 파라미터 설정 (필요 시 인자로 빼서 확장 가능)
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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