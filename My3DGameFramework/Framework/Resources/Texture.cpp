#include "EnginePch.h"
#include "Texture.h"

TexturePtr Texture::s_white = nullptr;
TexturePtr Texture::s_black = nullptr;
TexturePtr Texture::s_blue = nullptr;
TexturePtr Texture::s_gray = nullptr;

Texture::Texture()
{
    m_target = GL_TEXTURE_2D;
}
Texture::~Texture() = default;

TexturePtr Texture::Load(const TextureDesc& desc)
{
    std::string actualPath = RESOURCE.ResolvePath(desc.path);

    // 1. 객체 생성
    auto texture = TexturePtr(new Texture());
    texture->m_desc = desc;

    // 2. KTX 데이터를 통한 초기화
    if (!texture->InitFromKtx(actualPath)) return nullptr;

    LOG_INFO("Loading Texture: [{}] -> {}", desc.name, actualPath);
    return texture;
}

TexturePtr Texture::Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type)
{
    // TODO : 이름이랑 경로는 어떻게 해야 할지 고민 필요
    auto texture = TexturePtr(new Texture());
    texture->m_desc.name = fmt::format("Procedural_Tex_{}", (uintptr_t)texture.get());
    texture->m_desc.path = "@Virtual/Texture";
    texture->InitProcedural(width, height, internalFormat, format, type);
    return texture;
}

bool Texture::InitFromKtx(const std::string& path)
{
    ktxTexture2* kTexture = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromNamedFile
    (
        path.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("Texture: KTX2 Load Failed. Path: {}", path);
        return false;
    }

    // BasisU 트랜스코딩
    if (ktxTexture2_NeedsTranscoding(kTexture))
        ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);

    // 포맷 결정
    m_internalFormat = MapVkFormatToGLInternal(kTexture->vkFormat);
    bool isCompressed = kTexture->isCompressed;

    // GL 리소스 생성 및 할당
    glGenTextures(1, &m_texture);
    glBindTexture(m_target, m_texture);

    // Immutable Storage 할당
    glTexStorage2D
    (
        m_target, 
        kTexture->numLevels, 
        m_internalFormat,
        kTexture->baseWidth, kTexture->baseHeight
    );

    // 데이터 업로드 (밉맵 포함)
    UploadKtxData(kTexture);

    // 멤버 변수 업데이트
    m_width = kTexture->baseWidth;
    m_height = kTexture->baseHeight;
    m_internalFormat = m_internalFormat;

    ktxTexture_Destroy(ktxTexture(kTexture));
    return true;
}

void Texture::InitProcedural
(
    int32 width, int32 height, 
    uint32 internalFormat, uint32 format, 
    uint32 type
)
{
    glGenTextures(1, &m_texture);

    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_format = format;
    m_type = type;

    glBindTexture(m_target, m_texture);
    glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, type, nullptr);

    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::UploadKtxData(ktxTexture2* kTexture)
{
    bool isCompressed = kTexture->isCompressed;

    for (uint32 level = 0; level < kTexture->numLevels; ++level)
    {
        ktx_size_t offset;
        ktxTexture_GetImageOffset(ktxTexture(kTexture), level, 0, 0, &offset);

        int32 lw = std::max(1, (int32)kTexture->baseWidth >> level);
        int32 lh = std::max(1, (int32)kTexture->baseHeight >> level);
        uint32 size = static_cast<uint32>(ktxTexture_GetImageSize(ktxTexture(kTexture), level));

        if (isCompressed)
        {
            glCompressedTexSubImage2D(m_target, level, 0, 0, lw, lh, m_internalFormat, size, kTexture->pData + offset);
        }
        else
        {
            // HDR(Half Float) 대응을 위한 타입 체크
            GLenum type = (kTexture->vkFormat == 97) ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE;
            glTexSubImage2D(m_target, level, 0, 0, lw, lh, GL_RGBA, type, kTexture->pData + offset);
        }
    }
}

/*=============================//
//   default texture setters   //
//=============================*/
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

void Texture::SetSubData
(
    int32 x, int32 y, 
    int32 width, int32 height, 
    const void* data
)
{
    Bind();
    glTexSubImage2D(m_target, 0, x, y, width, height, m_format, m_type, data);
}

void Texture::SetData(const void* data, uint32 size)
{
    Bind();
    glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, data);
}

void Texture::SetTextureFormat
(
    int32 width, int32 height, 
    uint32 internalFormat, uint32 format, 
    uint32 type)

{
    m_width  = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_format = format;
    m_type   = type;

    Bind();
    glTexImage2D
    (
        m_target, 
        0,
        m_internalFormat, 
        m_width, m_height, 0, m_format, m_type, 
        nullptr
    );
}

/*====================================//
//   Texture default static methods   //
//====================================*/
TexturePtr Texture::CreateSolid(const std::vector<uint8>& rgba)
{
    auto tex = Texture::Create(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    tex->SetData(rgba.data());
    return tex;
}
TexturePtr Texture::GetWhiteTexture() 
{
    if (!s_white) s_white = CreateSolid({ 255, 255, 255, 255 }); return s_white; 
}
TexturePtr Texture::GetBlackTexture() 
{
    if (!s_black) s_black = CreateSolid({ 0, 0, 0, 255 }); return s_black; 
}
TexturePtr Texture::GetBlueTexture() 
{
    if (!s_blue) s_blue = CreateSolid({ 128, 128, 255, 255 }); return s_blue;
}
TexturePtr Texture::GetGrayTexture()
{
    if (!s_gray) s_gray = CreateSolid({ 128, 128, 128, 255 }); return s_gray;
}