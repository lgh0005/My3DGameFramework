#include "EnginePch.h"
#include "FramebufferBase.h"
#include "Resources/Texture.h"

FramebufferBase::~FramebufferBase()
{
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    if (m_depthBuffer) glDeleteRenderbuffers(1, &m_depthBuffer);
}

void FramebufferBase::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferBase::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

const TexturePtr FramebufferBase::GetColorAttachment(int32 index) const
{
    return GetTexture(index);
}

const TexturePtr FramebufferBase::GetTexture(int32 index) const
{
    if (index < 0 || index >= m_textures.size()) return nullptr;
    return m_textures[index];
}