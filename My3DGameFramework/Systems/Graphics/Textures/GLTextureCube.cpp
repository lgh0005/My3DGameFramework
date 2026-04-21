#include "GraphicsPch.h"
#include "GLTextureCube.h"
#include "GraphicsUtils/TextureUtils.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTextureCube::GLTextureCube() = default;
	GLTextureCube::~GLTextureCube() = default;

    GLTextureCubePtr GLTextureCube::Create(ktxTexture2* ktx)
    {
        if (ktx == nullptr) return nullptr;
        auto texture = GLTextureCubePtr(new GLTextureCube());
        texture->SetKtxTexture(ktx);
        texture->SetState(EResourceState::Loaded);
        return texture;
    }

    GLTextureCubePtr GLTextureCube::Create(uint32 size, uint32 vkFormat, uint32 levels)
    {
        auto texture = GLTextureCubePtr(new GLTextureCube());
        texture->m_size = size;
        texture->m_vkFormat = vkFormat;
        texture->m_levels = levels;
        texture->SetState(EResourceState::Loaded);
        return texture;
    }

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
        bool success = CommonUtils::Select
        (
            m_ktxTexture != nullptr,
            CreateFromKtx(),
            AllocateStorage(m_size, m_vkFormat, m_levels)
        );

        if (success) m_state = EResourceState::Ready;
        return success;
    }

    bool GLTextureCube::CreateFromKtx()
    {
        uint32 size = m_ktxTexture->baseWidth;
        uint32 vkFormat = m_ktxTexture->vkFormat;
        uint32 levels = m_ktxTexture->numLevels;
        bool isCompressed = m_ktxTexture->isCompressed;

        if (!AllocateStorage(size, vkFormat, levels))
            return false;

        uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
        uint32 format = TextureUtils::GetPixelFormatFromInternal(internalFormat);
        uint32 type = TextureUtils::GetGLDataTypeFromVk(vkFormat);

        for (uint32 level = 0; level < levels; ++level)
        {
            uint32 levelSize = Math::Max(1u, size >> level);

            for (uint32 face = 0; face < 6; ++face)
            {
                ktx_size_t offset;
                if (ktxTexture_GetImageOffset(ktxTexture(m_ktxTexture), level, 0, face, &offset) != KTX_SUCCESS) break;

                void* data = ktxTexture_GetData(ktxTexture(m_ktxTexture)) + offset;
                ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(m_ktxTexture), level);

                // DSA에서 큐브맵은 SubImage3D를 사용하며 face가 z-offset이 됨
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

        ktxTexture_Destroy(ktxTexture(m_ktxTexture));
        m_ktxTexture = nullptr;
        return true;
    }

    bool GLTextureCube::AllocateStorage(uint32 size, uint32 vkFormat, uint32 levels)
    {
        if (size <= 0) return false;

        m_target = GL_TEXTURE_CUBE_MAP;
        m_size = size;

        uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
        uint32 mipmapLevel = CommonUtils::Select
        (
            levels == 0,
            TextureUtils::CalculateMaxMipLevels(m_size, m_size),
            levels
        );

        glCreateTextures(m_target, 1, &m_handle);
        if (m_handle == 0) return false;

        // 큐브맵은 내부적으로 6개 면을 가지므로 glTextureStorage2D로 일괄 할당 가능
        glTextureStorage2D(m_handle, mipmapLevel, internalFormat, m_size, m_size);

        // 큐브맵 필수 파라미터 (Clamp to Edge)
        glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return true;
    }
}