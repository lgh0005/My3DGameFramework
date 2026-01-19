#include "EnginePch.h"
#include "MSAAFramebuffer.h"
#include "Resources/Textures/Texture.h"

MSAAFramebuffer::MSAAFramebuffer() = default;
MSAAFramebuffer::~MSAAFramebuffer()
{
    if (m_msaaFbo) glDeleteFramebuffers(1, &m_msaaFbo);
}

MSAAFramebufferUPtr MSAAFramebuffer::Create(int32 width, int32 height, int32 samples)
{
    auto fbo = MSAAFramebufferUPtr(new MSAAFramebuffer());
    fbo->m_attachmentInfos.push_back({ GL_RGBA16F, GL_RGBA, GL_FLOAT });
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

    // MRT 대응: 설정된 모든 컬러 어태치먼트를 순차적으로 복사
    for (int32 i = 0; i < (int32)m_msaaColorTextures.size(); ++i)
    {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

        glBlitFramebuffer(
            0, 0, m_width, m_height,
            0, 0, m_width, m_height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );
    }

    // 컬러 버퍼 복사 및 깊이 정보도 텍스처로 복사
    glBlitFramebuffer
    (
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );

    // 상태 복구
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool MSAAFramebuffer::Init(int32 width, int32 height, int32 samples)
{
    m_width = width;
    m_height = height;
    m_samples = samples;

    // 1. FBO ID들 생성
    glGenFramebuffers(1, &m_msaaFbo); // 그리기용(MSAA)
    glGenFramebuffers(1, &m_fbo);     // 결과 저장용(Resolve - 부모 멤버)

    return CreateAttachments();
}

bool MSAAFramebuffer::CreateAttachments()
{
    ClearAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Attach 텍스쳐 생성 (깊이와 색상)
    for (int32 i = 0; i < (int32)m_attachmentInfos.size(); ++i)
    {
        auto& info = m_attachmentInfos[i];
        CreateAndAttachColor(i, info.internalFormat, info.format, info.type, GL_LINEAR);
    }
    CreateAndAttachDepth();

    // MSAA FBO 설정 (멀티샘플 텍스처)
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    m_msaaColorTextures.clear();

    for (int32 i = 0; i < (int32)m_attachmentInfos.size(); ++i)
    {
        auto& info = m_attachmentInfos[i];
        auto msTex = Texture::CreateMultisample(m_width, m_height, m_samples, info.internalFormat);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, msTex->Get(), 0);
        m_msaaColorTextures.push_back(std::move(msTex));
    }

    // MSAA용 깊이-스텐실 텍스처
    m_msaaDepthTexture = Texture::CreateMultisample(m_width, m_height, m_samples, GL_DEPTH24_STENCIL8);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_msaaDepthTexture->Get(), 0);

    // MRT Draw Buffers 활성화
    std::vector<uint32> drawBuffers;
    for (int32 i = 0; i < (int32)m_msaaColorTextures.size(); ++i)
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());

    // 5. 상태 체크
    return CheckFramebufferStatus("MSAAFramebuffer");
}

