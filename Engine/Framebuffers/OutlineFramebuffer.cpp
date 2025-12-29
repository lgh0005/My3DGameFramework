#include "EnginePch.h"
#include "OutlineFramebuffer.h"
#include "Resources/Texture.h"

OutlineFramebuffer::OutlineFramebuffer() = default;
OutlineFramebuffer::~OutlineFramebuffer() = default;

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
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // 1. Color Attachment (마스크 및 색상 정보 저장)
    // GL_RGB 혹은 GL_RGBA 사용 (색상도 저장해야 하므로)
    auto texture = Texture::Create(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);
    m_textures.push_back(std::move(texture));

    // [중요] Depth Buffer는 필요 없습니다! 
    // 이유: 우리는 "Always On Top (X-Ray)" 효과를 낼 거라서 깊이 테스트를 끌 것이기 때문입니다.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}