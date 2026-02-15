#include "EnginePch.h"
#include "TextureBase.h"

TextureBase::TextureBase() = default;
TextureBase::~TextureBase()
{
	if (m_texture)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void TextureBase::Bind() const
{
	glBindTexture(m_target, m_texture);
}

void TextureBase::Unbind() const
{
	glBindTexture(m_target, 0);
}

void TextureBase::GenerateMipmap() const
{
    if (m_texture == 0) return;

    Bind();
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(m_target);
}

uint32 TextureBase::MapVkFormatToGLInternal(uint32 vkFormat)
{
    // libktx (Vulkan 포맷 번호) -> OpenGL Internal Format 매핑
    switch (vkFormat)
    {
        // 1. BC1 (DXT1) - 주로 불투명 텍스처
    case 131: // VK_FORMAT_BC1_RGB_UNORM_BLOCK
        return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case 133: // VK_FORMAT_BC1_RGB_SRGB_BLOCK
        return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;

        // 2. BC3 (DXT5) - 투명도(Alpha)가 포함된 텍스처
    case 137: // VK_FORMAT_BC3_UNORM_BLOCK
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case 139: // VK_FORMAT_BC3_SRGB_BLOCK
        return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

        // 3. BC7 (BPTC) - 고품질 압축 (현세대 표준)
    case 145: // VK_FORMAT_BC7_UNORM_BLOCK
        return GL_COMPRESSED_RGBA_BPTC_UNORM;
    case 146: // VK_FORMAT_BC7_SRGB_BLOCK
        return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;

        // 4. 비압축 RGBA8
    case 37:  // VK_FORMAT_R8G8B8A8_UNORM
        return GL_RGBA8;
    case 43:  // VK_FORMAT_R8G8B8A8_SRGB
        return GL_SRGB8_ALPHA8;

        // 5. HDR (High Dynamic Range)
    case 97:  // VK_FORMAT_R16G16B16A16_SFLOAT
        return GL_RGBA16F;

    default:
        LOG_WARN("TextureBase: Unknown vkFormat {}. Falling back to GL_RGBA8", vkFormat);
        return GL_RGBA8;
    }
}

uint32 TextureBase::GetPixelFormatFromInternal(uint32 internalFormat)
{
    // 내부 포맷을 바탕으로 외부 레이아웃(Pixel Format)을 유추합니다.
    switch (internalFormat)
    {
    case GL_SRGB8_ALPHA8:
    case GL_RGBA8:
    case GL_RGBA16F:
    case GL_RGBA32F:
    case GL_COMPRESSED_RGBA_BPTC_UNORM:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
        return GL_RGBA;

    case GL_R16F:
    case GL_R32F:
    case GL_RED:
        return GL_RED;

    case GL_RGB8:
    case GL_SRGB8:
    case GL_RGB16F:
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
    default:
        return GL_RGB;
    }
}

uint32 TextureBase::GetGLDataTypeFromVk(uint32 vkFormat)
{
    return (vkFormat == 97) ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE;
}
