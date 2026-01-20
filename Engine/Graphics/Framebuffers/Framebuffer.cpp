#include "EnginePch.h"
#include "Framebuffer.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(Framebuffer)

const TexturePtr Framebuffer::GetColorAttachment(int32 index) const
{
    return GetTexture(index);
}

const TexturePtr Framebuffer::GetDepthAttachment() const
{
    return m_depthTexture;
}

const TexturePtr Framebuffer::GetTexture(int32 index) const
{
    if (index < 0 || index >= m_textures.size()) return nullptr;
    return m_textures[index];
}

void Framebuffer::ClearAttachments()
{
    m_textures.clear();
    m_depthTexture = nullptr;
}

void Framebuffer::AttachColorTexture(uint32 index, TexturePtr texture)
{
    if (!texture) return;

    // 1. OpenGL FBO 슬롯에 텍스처 연결
    Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->Get(), 0);
    if (index >= m_textures.size()) m_textures.resize(index + 1, nullptr);
    m_textures[index] = texture;

    // 2. DrawBuffers 갱신
    std::vector<uint32> drawBuffers;
    for (int i = 0; i < m_textures.size(); ++i)
    {
        if (m_textures[i]) drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());

    BindToDefault();
}

void Framebuffer::AttachDepthTexture(TexturePtr texture)
{
    if (!texture) return;

    Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->Get(), 0);
    m_depthTexture = texture;
    BindToDefault();
}

bool Framebuffer::OnResize(int32 width, int32 height)
{
    if (m_width == width && m_height == height) return true;

    m_width = width;
    m_height = height;

    ClearAttachments();
    return CreateAttachments();
}

TexturePtr Framebuffer::CreateAndAttachColor(uint32 index, uint32 internalFormat, uint32 format, uint32 type, uint32 filter)
{
    TexturePtr tex = Texture::Create(m_width, m_height, internalFormat, format, type);
    tex->SetFilter(filter, filter);
    tex->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, tex->Get(), 0);

    m_textures.push_back(tex);
    return tex;
}

TexturePtr Framebuffer::CreateAndAttachDepth(uint32 internalFormat, uint32 format, uint32 type, uint32 filter)
{
    TexturePtr tex = Texture::Create(m_width, m_height, internalFormat, format, type);
    tex->SetFilter(filter, filter);
    tex->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex->Get(), 0);

    m_depthTexture = tex;
    return tex;
}
