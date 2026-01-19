#include "EnginePch.h"
#include "CubeTexture.h"
#include "Resources/Image.h"

CubeTexture::CubeTexture() = default;
CubeTexture::~CubeTexture()
{
    if (m_texture) glDeleteTextures(1, &m_texture);
}

/*=========================================//
//   default cube texture create methods   //
//=========================================*/
CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> images) 
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images)) return nullptr;
    return std::move(texture);
}

CubeTextureUPtr CubeTexture::CreateFromKtxImage(const std::string& ktxFilePath)
{
    return LoadKtx(ktxFilePath);
}

CubeTextureUPtr CubeTexture::Create(int32 width, int32 height, uint32 format, uint32 type)
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->Init(width, height, format, type);
    return std::move(texture);
}

/*==================================//
//   default cube texture setters   //
//==================================*/
void CubeTexture::Bind() const 
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

void CubeTexture::GenerateMipmap() const
{
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

/*========================//
//   cube texture utils   //
//========================*/
void CubeTexture::Init(int32 width, int32 height, uint32 format, uint32 type)
{
    glGenTextures(1, &m_texture);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;

    // 데이터 포맷 유추 (Internal Format으로부터 기본 Format 추출)
    GLenum pixelFormat = GL_RGB;
    if (m_format == GL_RGBA || m_format == GL_RGBA16F || m_format == GL_RGBA32F) pixelFormat = GL_RGBA;
    else if (m_format == GL_RED || m_format == GL_R16F) pixelFormat = GL_RED;

    // 6면 메모리 할당 (데이터는 nullptr)
    for (uint32 i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format,
            m_width, m_height, 0, pixelFormat, m_type, nullptr);
    }
}

bool CubeTexture::InitFromImages(const std::vector<Image*> images) 
{
    glGenTextures(1, &m_texture);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLenum internalFormat, format, type;
    GetFormatsFromImage(images[0], internalFormat, format, type);

    // 멤버 변수 저장
    m_width = images[0]->GetWidth();
    m_height = images[0]->GetHeight();
    m_type = type;
    m_format = internalFormat;

    for (uint32 i = 0; i < (uint32)images.size(); i++) 
    {
        glTexImage2D
        (
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat, 
            m_width, m_height,
            0,
            format,
            type,
            images[i]->GetData()
        );
    }

    return true;
}

CubeTextureUPtr CubeTexture::LoadKtx(const std::string& ktxFilePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;
    GLuint textureID = 0;
    GLenum target;
    GLenum glerror;

    // 1. KTX 파일 로드
    result = ktxTexture_CreateFromNamedFile
    (
        ktxFilePath.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("Failed to load KTX cubemap: {}", ktxFilePath);
        return nullptr;
    }

    // 2. 큐브맵 검증
    if (!kTexture->isCubemap)
    {
        LOG_ERROR("Texture is not a cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    // 3. GPU 업로드
    result = ktxTexture_GLUpload(kTexture, &textureID, &target, &glerror);
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("Failed to upload KTX cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->m_texture = textureID;
    texture->m_width = kTexture->baseWidth;
    texture->m_height = kTexture->baseHeight;

    GLint format = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    texture->m_format = format;
    texture->m_type = GL_UNSIGNED_BYTE;

    // 4. 공통 파라미터 설정
    // 큐브맵은 HDR이든 아니든 모서리 솔기(Seam) 방지를 위해 무조건 Clamp To Edge.
    texture->Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    ktxTexture_Destroy(kTexture);
    return std::move(texture);
}

void CubeTexture::GetFormatsFromImage(const Image* image, 
    GLenum& outInternal, GLenum& outFormat, GLenum& outType)
{
    // 1. 데이터 타입 결정 (Byte vs Float)
    bool isHDR = (image->GetBytePerChannel() == 4);
    outType = isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

    // 2. 포맷 결정
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount())
    {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
    }
    outFormat = format;

    // 3. 내부 포맷(Internal Format) 결정
    if (isHDR)
    {
        if (format == GL_RGB) outInternal = GL_RGB16F;
        else if (format == GL_RGBA) outInternal = GL_RGBA16F;
        else if (format == GL_RED) outInternal = GL_R16F;
        else outInternal = format; // Fallback
    }
    else
    {
        outInternal = format; // LDR은 기본 포맷 그대로 사용 (RGB8 등)
    }
}
