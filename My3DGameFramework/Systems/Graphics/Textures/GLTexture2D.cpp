#include "GraphicsPch.h"
#include "GLTexture2D.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTexture2D::GLTexture2D() = default;
	GLTexture2D::~GLTexture2D() = default;

	GLTexture2DPtr GLTexture2D::Create(ktxTexture* ktx)
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
		uint32 internalFormat, uint32 levels
	)
	{
		auto texture = SharedPtr<GLTexture2D>(new GLTexture2D());
		texture->m_width = width;
		texture->m_height = height;
		texture->m_internalFormat = internalFormat;
		texture->m_levels = levels;
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
		bool success = false;
		if (m_ktxTexture != nullptr) success = CreateFromKtx();
		else success = AllocateStorage(m_width, m_height, m_internalFormat, m_levels);

		if (success) m_state = EResourceState::Ready;
		return success;
	}

	bool GLTexture2D::CreateFromKtx()
	{
		if (m_ktxTexture == nullptr) return false;

		uint32 width = m_ktxTexture->baseWidth;
		uint32 height = m_ktxTexture->baseHeight;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;

		uint32 internalFormat = 0;
		uint32 format = 0;
		uint32 type = 0;

		if (m_ktxTexture->classId == ktxTexture1_c)
		{
			ktxTexture1* ktx1 = reinterpret_cast<ktxTexture1*>(m_ktxTexture);
			internalFormat = ktx1->glInternalformat;
			format = ktx1->glFormat;
			type = ktx1->glType;
		}
		else if (m_ktxTexture->classId == ktxTexture2_c)
		{
			ktxTexture2* ktx2 = reinterpret_cast<ktxTexture2*>(m_ktxTexture);
			uint32 vkFormat = ktx2->vkFormat;
			internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
			format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
			type = TextureUtils::GetGLDataTypeFromVk(vkFormat);
		}
		else return false;

		if (!AllocateStorage(width, height, internalFormat, levels))
			return false;

		for (uint32 level = 0; level < levels; ++level)
		{
			ktx_size_t offset;
			if (ktxTexture_GetImageOffset(m_ktxTexture, level, 0, 0, &offset) != KTX_SUCCESS) break;

			uint32 levelWidth = Math::Max(1u, width >> level);
			uint32 levelHeight = Math::Max(1u, height >> level);
			void* data = ktxTexture_GetData(m_ktxTexture) + offset;
			ktx_size_t imageSize = ktxTexture_GetImageSize(m_ktxTexture, level);

			if (isCompressed)
				glCompressedTextureSubImage2D(m_handle, level, 0, 0, levelWidth, levelHeight, internalFormat, static_cast<GLsizei>(imageSize), data);
			else
				glTextureSubImage2D(m_handle, level, 0, 0, levelWidth, levelHeight, format, type, data);
		}

		ktxTexture_Destroy(m_ktxTexture);
		m_ktxTexture = nullptr;
		return true;
	}

	bool GLTexture2D::AllocateStorage(uint32 width, uint32 height, uint32 internalFormat, uint32 levels)
	{
		if (width <= 0 || height <= 0) return false;

		m_target = GL_TEXTURE_2D;
		m_width = width;
		m_height = height;

		uint32 mipmapLevel = CommonUtils::Select(levels == 0, TextureUtils::CalculateMaxMipLevels(m_width, m_height), levels);

		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// Immutable Storage 할당
		glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_width, m_height);
		GLint minFilter = (mipmapLevel == 1) ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;

		// 기본 파라미터 설정 (필요 시 인자로 빼서 확장 가능)
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, minFilter);
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