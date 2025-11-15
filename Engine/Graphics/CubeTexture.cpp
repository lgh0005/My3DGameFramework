#include "EnginePch.h"
#include "CubeTexture.h"
#include "Graphics/Image.h"

CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> images) 
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images)) return nullptr;
    return std::move(texture);
}

CubeTexture::~CubeTexture() 
{
    if (m_texture) 
        glDeleteTextures(1, &m_texture);
}

void CubeTexture::Bind() const 
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
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

    for (uint32 i = 0; i < (uint32)images.size(); i++) 
    {
        auto image = images[i];
        GLenum format = GL_RGBA;
        switch (image->GetChannelCount()) 
        {
            case 1: format = GL_RED; break;
            case 2: format = GL_RG; break;
            case 3: format = GL_RGB; break;
            default: break;
        }

        glTexImage2D
        (
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
            image->GetWidth(), image->GetHeight(), 0,
            format, GL_UNSIGNED_BYTE,
            image->GetData()
        );
    }

    return true;
}