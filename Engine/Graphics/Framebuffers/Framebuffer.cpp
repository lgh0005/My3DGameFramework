#include "EnginePch.h"
#include "Framebuffer.h"
#include "Resources/Textures/Texture.h"

Framebuffer::Framebuffer() = default;
Framebuffer::~Framebuffer()
{
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::Blit(Framebuffer* src, Framebuffer* dst, uint32 mask, uint32 filter)
{
    if (!src || !dst) return;
    int32 srcW = src->GetWidth(); int32 srcH = src->GetHeight();
    int32 dstW = dst->GetWidth(); int32 dstH = dst->GetHeight();

    BlitRegion(src, 0, 0, srcW, srcH, dst, 0, 0, dstW, dstH, mask, filter);
}

void Framebuffer::BlitRegion
(
    Framebuffer* src, int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1,
    Framebuffer* dst, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1,
    uint32 mask, uint32 filter
)
{
    if (!src || !dst) return;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->Get());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->Get());
    glBlitFramebuffer
    (
        srcX0, srcY0, srcX1, srcY1,
        dstX0, dstY0, dstX1, dstY1,
        mask,
        filter
    );

    BindToDefault();
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

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

bool Framebuffer::CheckFramebufferStatus(const std::string& name) const
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    BindToDefault();
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("{} Framebuffer Incomplete! Error Code: 0x{:X}", name, status);
        return false;
    }
    return true;
}