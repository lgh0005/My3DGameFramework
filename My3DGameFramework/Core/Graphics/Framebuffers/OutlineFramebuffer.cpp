#include "EnginePch.h"
#include "OutlineFramebuffer.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(OutlineFramebuffer)

OutlineFramebufferUPtr OutlineFramebuffer::Create(int32 width, int32 height)
{
    auto fbo = OutlineFramebufferUPtr(new OutlineFramebuffer());
    if (!fbo->Init(width, height)) return nullptr;
    return std::move(fbo);
}

bool OutlineFramebuffer::Init(int32 width, int32 height)
{
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_fbo);
    return CreateAttachments();
}

bool OutlineFramebuffer::CreateAttachments()
{
    ClearAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // 1. Color Attachment (마스크 및 색상 정보 저장)
    // GL_RGB 혹은 GL_RGBA 사용 (색상도 저장해야 하므로)
    CreateAndAttachColor(0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    // 2. 결과 출력
    return CheckFramebufferStatus();
}
