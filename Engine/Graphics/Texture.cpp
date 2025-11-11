#include "EnginePch.h"
#include "Texture.h"
#include "Graphics/Image.h"

TextureUPtr Texture::Create(int32 width, int32 height, uint32_t format)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

TextureUPtr Texture::CreateFromImage(const Image* image)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture()
{
    if (m_texture)
        glDeleteTextures(1, &m_texture);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32 minFilter, uint32 magFilter) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32 sWrap, uint32 tWrap) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture);
    Bind();
    SetFilter(GL_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image)
{
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount())
    {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        default: break;
    }

    m_width = image->GetWidth();
    m_height = image->GetHeight();
    m_format = format;

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0,
        format, GL_UNSIGNED_BYTE, image->GetData());

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::SetTextureFormat(int32 width, int32 height, uint32 format)
{
    m_width = width;
    m_height = height;
    m_format = format;

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0,
                 m_format, GL_UNSIGNED_BYTE, nullptr);
}
