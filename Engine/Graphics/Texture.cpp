#include "EnginePch.h"
#include "Texture.h"
#include "Graphics/Image.h"

TextureUPtr Texture::Create(int32 width, int32 height, uint32 format, uint32 type)
{
    return Create(width, height, GL_RGBA16F, format, GL_FLOAT);
}

TextureUPtr Texture::Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, internalFormat, format, type);
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

TextureUPtr Texture::CreateFromKtx(const std::string& ktxFilePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;
    GLuint textureID = 0;
    GLenum target;
    GLenum glerror;

    // 1. ktx 파일 로드
    result = ktxTexture_CreateFromNamedFile
    (
        ktxFilePath.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );
    if (result != KTX_SUCCESS)
    {
        SPDLOG_ERROR("Failed to load KTX texture: {}", ktxFilePath);
        return nullptr;
    }

    // 2. GPU 업로드
    result = ktxTexture_GLUpload(kTexture, &textureID, &target, &glerror);
    if (result != KTX_SUCCESS)
    {
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    auto texture = TextureUPtr(new Texture());
    texture->m_texture = textureID;
    texture->m_width = kTexture->baseWidth;
    texture->m_height = kTexture->baseHeight;
    texture->m_target = target;

    GLint format = 0;
    glBindTexture(target, textureID);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    texture->m_format = format;

    // 3. 메모리 해제
    ktxTexture_Destroy(kTexture);

    return std::move(texture);
}

Texture::~Texture()
{
    if (m_texture)
        glDeleteTextures(1, &m_texture);
}

void Texture::Bind() const
{
    glBindTexture(m_target, m_texture);
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

void Texture::SetData(const void* data, uint32 size)
{
    // size는 검증용으로 쓸 수 있지만 여기선 단순하게 전체 업로드라고 가정
    Bind();
    glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, data);
}

void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture);
    m_target = GL_TEXTURE_2D;
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
    m_type = GL_UNSIGNED_BYTE;

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0,
        format, m_type, image->GetData());

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::SetTextureFormat(int32 width, int32 height, 
    uint32 internalFormal, uint32 format, uint32 type)
{
    m_width  = width;
    m_height = height;
    m_format = format;
    m_type   = type;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormal, m_width, m_height, 0,
        format, m_type, nullptr);
}
