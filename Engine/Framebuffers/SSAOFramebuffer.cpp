#include "EnginePch.h"
#include "SSAOFramebuffer.h"
#include "Resources/Texture.h"

DECLARE_DEFAULTS_IMPL(SSAOFramebuffer)

SSAOFramebufferUPtr SSAOFramebuffer::Create(int32 width, int32 height)
{
    auto fbo = SSAOFramebufferUPtr(new SSAOFramebuffer());
    if (!fbo->Init(width, height)) return nullptr;
    return std::move(fbo);
}

bool SSAOFramebuffer::Init(int32 width, int32 height)
{
    m_width = width;
    m_height = height;

    // 1. 부모의 m_fbo 사용
    glGenFramebuffers(1, &m_fbo);

    return CreateAttachments();
}

bool SSAOFramebuffer::CreateAttachments()
{
    ClearAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // 1. SSAO용 텍스처 생성 (R16F)
    CreateAndAttachColor(0, GL_R16F, GL_RED, GL_FLOAT, GL_NEAREST);

    // 2. Draw Buffer 설정
    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // 3. 상태 확인 및 언바인드
    return CheckFramebufferStatus("SSAO");
}
