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