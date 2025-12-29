#include "EnginePch.h"
#include "Framebuffer.h"
#include "Resources/Texture.h"

Framebuffer::Framebuffer() = default;
Framebuffer::~Framebuffer()
{
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    if (m_depthBuffer) glDeleteRenderbuffers(1, &m_depthBuffer);
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

const TexturePtr Framebuffer::GetTexture(int32 index) const
{
    if (index < 0 || index >= m_textures.size()) return nullptr;
    return m_textures[index];
}