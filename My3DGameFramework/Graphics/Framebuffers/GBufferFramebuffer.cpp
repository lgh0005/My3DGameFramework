#include "EnginePch.h"
#include "GBufferFramebuffer.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(GBufferFramebuffer)

GBufferFramebufferUPtr GBufferFramebuffer::Create(int32 width, int32 height)
{
    auto gBuffer = GBufferFramebufferUPtr(new GBufferFramebuffer());
    if (!gBuffer->Init(width, height)) return nullptr;
    return std::move(gBuffer);
}

bool GBufferFramebuffer::Init(int32 width, int32 height)
{
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_fbo);

    return CreateAttachments();
}

bool GBufferFramebuffer::CreateAttachments()
{
    ClearAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    CreateAndAttachColor(0, GL_RGBA16F, GL_RGBA, GL_FLOAT);         // Position
    CreateAndAttachColor(1, GL_RGBA16F, GL_RGBA, GL_FLOAT);         // Normal
    CreateAndAttachColor(2, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo
    CreateAndAttachColor(3, GL_RGBA16F, GL_RGBA, GL_FLOAT);         // Emission
    CreateAndAttachColor(4, GL_RGBA16F, GL_RGBA, GL_FLOAT);         // Velocity

    // Draw Buffers 부착
    uint32 attachments[5] = 
    {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4
    };
    glDrawBuffers(5, attachments);

    // Depth Texture 사용
    CreateAndAttachDepth();

    // 5. 상태 체크
    return CheckFramebufferStatus();
}
