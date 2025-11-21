#include "EnginePch.h"
#include "FrameBuffer.h"
#include "Graphics/Texture.h"

FramebufferUPtr Framebuffer::Create(int32 width, int32 height, int32 samples)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->Init(width, height, samples)) return nullptr;
    return std::move(framebuffer);
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() 
{
    if (m_msaaDepthStencilBuffer)
        glDeleteRenderbuffers(1, &m_msaaDepthStencilBuffer);
    if (m_msaaColorBuffer)
        glDeleteRenderbuffers(1, &m_msaaColorBuffer);
    if (m_msaaFbo)
        glDeleteFramebuffers(1, &m_msaaFbo);
    if (m_resolveFbo)
        glDeleteFramebuffers(1, &m_resolveFbo);
}

void Framebuffer::Bind() const 
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
}

void Framebuffer::Resolve() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolveFbo);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

bool Framebuffer::Init(int32 width, int32 height, int32 samples)
{
    m_width = width;
    m_height = height;
    m_samples = samples;

    // 1. MSAA 프레임 버퍼 생성
    glGenFramebuffers(1, &m_msaaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);

    // 1-1. MSAA 컬러 렌더버퍼 생성 및 부착
    glGenRenderbuffers(1, &m_msaaColorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaColorBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA16F, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_msaaColorBuffer);

    // 1-2. MSAA 깊이/스텐실 렌더버퍼 생성 및 부착
    glGenRenderbuffers(1, &m_msaaDepthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

    // 1-3. MSAA FBO 완성 검사
    auto msaaResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (msaaResult != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create MSAA framebuffer: {}", msaaResult);
        return false;
    }

    // 2. 프레임 버퍼 리졸빙 (Multisample -> 일반 2D Sample로 변경)
    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    m_resolveTexture = Texture::Create(width, height, GL_RGBA);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resolveTexture->Get(), 0);

    auto resolveResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (resolveResult != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create Resolve framebuffer: {}", resolveResult);
        return false;
    }

    BindToDefault();

    return true;
}