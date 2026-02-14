#include "EnginePch.h"
#include "Texture.h"
#include "Resources/Image.h"

Texture::Texture()
{
    m_target = GL_TEXTURE_2D;
}
Texture::~Texture() = default;

TexturePtr Texture::Load(const TextureDesc& desc)
{
    // 1. 확장자 확인 및 이미지 로드
    std::string ext = std::filesystem::path(desc.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 2. KTX 파일인 경우 (Image 클래스를 거치지 않고 직접 로드)
    if (ext == ".ktx")
    {
        auto ktxTexture = TextureUtils::LoadTextureFromKtx(desc.path);
        if (ktxTexture)
        {
            ktxTexture->m_desc = desc;
            return ktxTexture;
        }
        return nullptr;
    }

    // 3. 일반 이미지 로드 (LDR/HDR)
    auto texture = TexturePtr(new Texture());
    texture->m_desc = desc;

    ImageDesc imgDesc(desc.path);
    imgDesc.isFlipY = true;

    auto image = Image::Load(imgDesc);
    if (!image)
    {
        LOG_ERROR("Failed to load texture image: {}", desc.path);
        return nullptr;
    }

    // 4. GPU 텍스처 생성
    texture->CreateTexture();

    // 5. 포맷 결정 (TextureUtils의 헬퍼 함수 활용)
    GLenum internalFormat, format, type;
    TextureUtils::GetFormatsFromImage(image.get(), internalFormat, format, type);
    if (desc.sRGB && image->GetBytePerChannel() == 1)
    {
        switch (format)
        {
            case GL_RGB: internalFormat = GL_SRGB8; break;
            case GL_RGBA: internalFormat = GL_SRGB8_ALPHA8; break;
        }
    }

    // 6. GPU 메모리 할당 및 데이터 전송
    texture->SetTextureFormat(image->GetWidth(), image->GetHeight(), internalFormat, format, type);
    texture->SetData(image->GetData());

    // 7. 밉맵 및 필터 설정
    if (desc.useMipMap)
    {
        texture->GenerateMipmap();
        texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    }
    else
    {
        texture->SetFilter(GL_LINEAR, GL_LINEAR);
    }
    texture->SetWrap(GL_REPEAT, GL_REPEAT);

    LOG_INFO("Texture Loaded: {} ({}x{})", desc.path, image->GetWidth(), image->GetHeight());
    return texture;
}

/*====================================//
//   default texture create methods   //
//====================================*/
TexturePtr Texture::Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type)
{
    auto texture = TexturePtr(new Texture());
    texture->m_desc.name = fmt::format("Procedural_Tex_{}", (uintptr_t)texture.get());
    texture->m_desc.path = "@Virtual/Texture";
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, internalFormat, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return texture;
}

/*=============================//
//   default texture setters   //
//=============================*/
void Texture::Bind() const
{
    glBindTexture(m_target, m_texture);
}

void Texture::GenerateMipmap() const
{
    Bind();
    glGenerateMipmap(m_target);
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
    glTexImage2D
    (
        m_target, 
        0,
        m_internalFormat, 
        m_width, m_height, 0, m_format, m_type, 
        nullptr
    );
}