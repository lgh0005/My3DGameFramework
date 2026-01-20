#include "EnginePch.h"
#include "Texture.h"
#include "Resources/Image.h"

Texture::Texture()
{
    m_target = GL_TEXTURE_2D;
}
Texture::~Texture() = default;

/*====================================//
//   default texture create methods   //
//====================================*/
TextureUPtr Texture::Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, internalFormat, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

TextureUPtr Texture::CreateMultisample(int32 width, int32 height, int32 samples, uint32 internalFormat)
{
    auto texture = TextureUPtr(new Texture());

    // 1. ID 생성 및 타겟 설정
    glGenTextures(1, &texture->m_texture);
    texture->m_target = GL_TEXTURE_2D_MULTISAMPLE;
    texture->m_samples = samples;
    texture->m_internalFormat = internalFormat;

    // 2. 메모리 할당 (SetTextureFormat 활용)
    texture->SetTextureFormat(width, height, internalFormat, 0, 0);

    return std::move(texture);
}

/*=============================//
//   default texture setters   //
//=============================*/
void Texture::Bind() const
{
    glBindTexture(m_target, m_texture);
}

void Texture::Resize(int32 width, int32 height)
{
    if (m_width == width && m_height == height) return;
    SetTextureFormat(width, height, m_internalFormat, m_format, m_type);
}

void Texture::SetFilter(uint32 minFilter, uint32 magFilter) const
{
    Bind();
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32 sWrap, uint32 tWrap) const
{
    Bind();
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::SetBorderColor(const glm::vec4& color) const 
{
    Bind();
    glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}

void Texture::SetSubData(int32 x, int32 y, int32 width, int32 height, const void* data)
{
    Bind();
    glTexSubImage2D(m_target, 0, x, y, width, height, m_format, m_type, data);
}

void Texture::SetData(const void* data, uint32 size)
{
    Bind();
    glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, data);
}

void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture);
    Bind();

    SetFilter(GL_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFormat(int32 width, int32 height, 
    uint32 internalFormat, uint32 format, uint32 type)
{
    m_width  = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_format = format;
    m_type   = type;

    Bind();
    
    if (m_target == GL_TEXTURE_2D_MULTISAMPLE)
    {
        // 멀티샘플 텍스처 전용 할당
        glTexImage2DMultisample(m_target, m_samples, m_internalFormat, m_width, m_height, GL_TRUE);
    }
    else
    {
        // 일반 2D 텍스처 할당
        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);
    }
}