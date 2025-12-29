#include "EnginePch.h"
#include "Texture.h"
#include "Resources/Image.h"

Texture::Texture() = default;
Texture::~Texture()
{
    if (m_texture) glDeleteTextures(1, &m_texture);
}

/*====================================//
//   default texture create methods   //
//====================================*/
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

TextureUPtr Texture::CreateFromHDR(const Image* image)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();

    GLenum internalFormat = GL_RGB16F;
    GLenum format = GL_RGB;

    // 채널 수에 따른 포맷 결정
    if (image->GetChannelCount() == 4)
    {
        internalFormat = GL_RGBA16F;
        format = GL_RGBA;
    }
    else if (image->GetChannelCount() == 3)
    {
        internalFormat = GL_RGB16F;
        format = GL_RGB;
    }

    // HDR 데이터는 float 타입이므로 GL_FLOAT 명시
    texture->SetTextureFormat(image->GetWidth(), image->GetHeight(),
        internalFormat, format, GL_FLOAT);

    // 데이터 업로드
    texture->SetData(image->GetData());

    // HDR용 파라미터 설정
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);

    return std::move(texture);
}

TextureUPtr Texture::CreateFromKtxImage(const std::string& ktxFilePath)
{
    // 1. 공통 로더 호출
    auto texture = LoadKtx(ktxFilePath);
    if (!texture) return nullptr;

    // 2. 일반 텍스쳐 설정: 반복(Repeat) 허용, Mipmap 사용
    texture->SetWrap(GL_REPEAT, GL_REPEAT);
    texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    return std::move(texture);
}

TextureUPtr Texture::CreateFromKtxHDR(const std::string& ktxFilePath)
{
    // 1. 공통 로더 호출
    auto texture = LoadKtx(ktxFilePath);
    if (!texture) return nullptr;

    // 2. HDR 설정: 모서리 Clamp 필수, Linear 필터링 (Mipmap은 상황에 따라 다름)
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);

    return std::move(texture);
}

/*=============================//
//   default texture setters   //
//=============================*/
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
    uint32 internalFormat, uint32 format, uint32 type)
{
    m_width  = width;
    m_height = height;
    m_format = format;
    m_type   = type;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0,
        format, m_type, nullptr);
}

TextureUPtr Texture::LoadKtx(const std::string& ktxFilePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;
    GLuint textureID = 0;
    GLenum target;
    GLenum glerror;

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

    // GPU 업로드
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

    // 포맷 정보 저장
    GLint format = 0;
    glBindTexture(target, textureID);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    texture->m_format = format;

    ktxTexture_Destroy(kTexture);
    return std::move(texture);
}

/*===================//
//   texture utils   //
//===================*/
TexturePtr Texture::s_whiteTex = nullptr;
TexturePtr Texture::s_grayTex = nullptr;
TexturePtr Texture::s_blackTex = nullptr;
TexturePtr Texture::s_blueTex = nullptr;

TexturePtr Texture::Create4x4Texture(const std::vector<uint8>& colorData)
{
    auto texture = Texture::Create(4, 4, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE);

    std::vector<uint8> fullBuffer;
    fullBuffer.reserve(4 * 4 * 4);

    for (int i = 0; i < 16; ++i)
        fullBuffer.insert(fullBuffer.end(), colorData.begin(), colorData.end());
    texture->SetData(fullBuffer.data(), (uint32)fullBuffer.size());

    return std::move(texture);
}

TexturePtr Texture::CreateWhite()
{
    if (s_whiteTex) return s_whiteTex;
    std::vector<uint8> data = { 255, 255, 255, 255 };
    s_whiteTex = Create4x4Texture(data);
    return s_whiteTex;
}

TexturePtr Texture::CreateGray()
{
    if (s_grayTex) return s_grayTex;
    std::vector<uint8> data = { 128, 128, 128, 255 };
    s_grayTex = Create4x4Texture(data);
    return s_grayTex;
}

TexturePtr Texture::CreateBlack()
{
    if (s_blackTex) return s_blackTex;
    std::vector<uint8> data = { 0, 0, 0, 255 };
    s_blackTex = Create4x4Texture(data);
    return s_blackTex;
}

TexturePtr Texture::CreateBlue()
{
    if (s_blueTex) return s_blueTex;
    std::vector<uint8> data = { 128, 128, 255, 255 };
    s_blueTex = Create4x4Texture(data);
    return s_blueTex;
}