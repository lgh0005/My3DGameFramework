#include "GraphicsPch.h"
#include "GLTexture3D.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTexture3D::GLTexture3D() = default;
	GLTexture3D::~GLTexture3D() = default;

	GLTexture3DPtr GLTexture3D::Create(ktxTexture* ktx)
	{
		if (ktx == nullptr) return nullptr;
		auto texture = SharedPtr<GLTexture3D>(new GLTexture3D());
		texture->SetKtxTexture(ktx);
		texture->SetState(EResourceState::Loaded);
		return texture;
	}

	GLTexture3DPtr GLTexture3D::Create(uint32 width, uint32 height, uint32 depth, uint32 internalFormat, uint32 levels)
	{
		auto texture = SharedPtr<GLTexture3D>(new GLTexture3D());
		texture->m_width = width;
		texture->m_height = height;
		texture->m_depth = depth;
		texture->m_internalFormat = internalFormat;
		texture->m_levels = levels;
		texture->SetState(EResourceState::Loaded);
		return texture;
	}

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
		bool success = false;
		if (m_ktxTexture != nullptr) success = CreateFromKtx();
		else success = AllocateStorage(m_width, m_height, m_depth, m_internalFormat, m_levels);

		if (success) m_state = EResourceState::Ready;
		return success;
	}

	bool GLTexture3D::CreateFromKtx()
	{
		uint32 width = m_ktxTexture->baseWidth;
		uint32 height = m_ktxTexture->baseHeight;
		uint32 depth = m_ktxTexture->baseDepth;
		uint32 levels = m_ktxTexture->numLevels;
		bool isCompressed = m_ktxTexture->isCompressed;
		uint32 internalFormat = 0, format = 0, type = 0;

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

		if (!AllocateStorage(width, height, depth, internalFormat, levels))
			return false;

		for (uint32 level = 0; level < levels; ++level)
		{
			ktx_size_t offset;
			if (ktxTexture_GetImageOffset(m_ktxTexture, level, 0, 0, &offset) != KTX_SUCCESS) break;

			uint32 levelWidth = Math::Max(1u, width >> level);
			uint32 levelHeight = Math::Max(1u, height >> level);
			uint32 levelDepth = Math::Max(1u, depth >> level);
			void* data = ktxTexture_GetData(m_ktxTexture) + offset;
			ktx_size_t imageSize = ktxTexture_GetImageSize(m_ktxTexture, level);

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

		ktxTexture_Destroy(ktxTexture(m_ktxTexture));
		m_ktxTexture = nullptr;
		return true;
	}

	bool GLTexture3D::AllocateStorage(uint32 width, uint32 height, uint32 depth, uint32 internalFormat, uint32 levels)
	{
		if (width <= 0 || height <= 0 || depth <= 0) return false;

		m_target = GL_TEXTURE_3D;
		m_width = width;
		m_height = height;
		m_depth = depth;

		uint32 mipmapLevel = (levels == 0) ? TextureUtils::CalculateMaxMipLevels(m_width, m_height, m_depth) : levels;

		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		glTextureStorage3D(m_handle, mipmapLevel, internalFormat, m_width, m_height, m_depth);

		// 3D 특화 파라미터 (Wrap R 추가)
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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