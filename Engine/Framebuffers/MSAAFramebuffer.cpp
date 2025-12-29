#include "EnginePch.h"
#include "MSAAFramebuffer.h"
#include "Resources/Texture.h"

MSAAFramebuffer::MSAAFramebuffer() = default;
MSAAFramebuffer::~MSAAFramebuffer()
{
    if (m_msaaFbo) glDeleteFramebuffers(1, &m_msaaFbo);
    if (m_msaaDepthStencilBuffer) glDeleteRenderbuffers(1, &m_msaaDepthStencilBuffer);
    if (!m_msaaColorBuffers.empty())
        glDeleteRenderbuffers((GLsizei)m_msaaColorBuffers.size(), m_msaaColorBuffers.data());
}

MSAAFramebufferUPtr MSAAFramebuffer::Create(int32 width, int32 height, int32 samples)
{
    auto fbo = MSAAFramebufferUPtr(new MSAAFramebuffer());
    if (!fbo->Init(width, height, samples)) return nullptr;
    return std::move(fbo);
}

void MSAAFramebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
}

void MSAAFramebuffer::Resolve() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // 컬러 버퍼 복사 (Linear 보간 안 됨, Nearest만 가능)
    // 여러 개의 Attachment가 있다면 반복문으로 처리 가능 (여기선 0번만 예시)
    glBlitFramebuffer
    (
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );

    // 상태 복구
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool MSAAFramebuffer::Init(int32 width, int32 height, int32 samples)
{
    m_width = width;
    m_height = height;
    m_samples = samples;

    // ====================================================
    // 1. MSAA FBO 생성 (Drawing Target)
    // ====================================================
    glGenFramebuffers(1, &m_msaaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);

    // 1-1. Multisample Color Renderbuffer
    uint32 colorBuffer;
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    // HDR을 위해 GL_RGBA16F 사용
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA16F, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
    m_msaaColorBuffers.push_back(colorBuffer);

    // 1-2. Multisample Depth Renderbuffer
    glGenRenderbuffers(1, &m_msaaDepthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

    // 1-3. 상태 확인
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    // ====================================================
    // 2. Resolve FBO 생성 (Texture Target - 부모 멤버 사용)
    // ====================================================
    // Forward Rendering 결과를 텍스처로 써먹으려면(PostProcess 등) 이게 필요함
    glGenFramebuffers(1, &m_fbo); // 부모 멤버 m_fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);

    // 부모의 텍스처 리스트에 등록
    m_textures.push_back(std::move(texture));

    // DrawBuffer 설정
    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}