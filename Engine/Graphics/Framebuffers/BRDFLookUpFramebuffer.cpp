#include "EnginePch.h"
#include "BRDFLookUpFramebuffer.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(BRDFLookUpFramebuffer)

BRDFLookUpFramebufferUPtr BRDFLookUpFramebuffer::Create(int32 width, int32 height)
{
    auto fbo = BRDFLookUpFramebufferUPtr(new BRDFLookUpFramebuffer());
    if (!fbo->Init(width, height)) return nullptr;
    return std::move(fbo);
}

bool BRDFLookUpFramebuffer::Init(int32 width, int32 height)
{
    m_width = width;
    m_height = height;

    // 1. FBO ID 생성 (객체 생명주기 동안 1회)
    glGenFramebuffers(1, &m_fbo);

    // 2. 실제 텍스처 생성 로직 호출
    return CreateAttachments();
}

bool BRDFLookUpFramebuffer::CreateAttachments()
{
    ClearAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // 2. 텍스처 생성 (R16F)
    auto texture = Texture::Create(m_width, m_height, GL_RG16F, GL_RG, GL_FLOAT);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);

    // 3. 부모의 텍스처 컨테이너에 등록
    m_textures.push_back(std::move(texture));

    // 4. Draw Buffer 설정
    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // 5. 상태 체크
    return CheckFramebufferStatus("BRDFLookupFramebuffer");
}
