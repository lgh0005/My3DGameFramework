#include "EnginePch.h"
#include "FrameBuffer.h"
#include "Graphics/Texture.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment) 
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment)) return nullptr;
    return std::move(framebuffer);
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() 
{
    if (m_depthStencilBuffer) 
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);

    if (m_framebuffer) 
        glDeleteFramebuffers(1, &m_framebuffer);
}

void Framebuffer::Bind() const 
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment) 
{
    // 1. 컬러 버퍼로 사용할 텍스쳐가 뭔지를 대입, 그 후 프레임버퍼로 바인딩
    m_colorAttachment = colorAttachment;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // 2. 프레임 버퍼에 텍스쳐를 부착
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        colorAttachment->Get(), 0);

    // 3. 깊이 버퍼와 스텐실 버퍼를 생성
    // 먼저 렌더 버퍼를 만들고 그 버퍼를 어떤 버퍼로 활용할 것인지를 명시하고 있다.
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);

    // 3-1. 깊이 버퍼 생성 및 바인딩
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        colorAttachment->GetWidth(), colorAttachment->GetHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // 3-2. 스텐실 버퍼 생성 및 바인딩
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    // 4.프레임 버퍼 생성 여부 체크
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) 
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}