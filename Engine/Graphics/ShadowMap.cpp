#include "EnginePch.h"
#include "ShadowMap.h"
#include "Resources/Texture.h"

ShadowMap::ShadowMap() = default;
ShadowMap::~ShadowMap()
{
    if (m_framebuffer) glDeleteFramebuffers(1, &m_framebuffer);
}

ShadowMapUPtr ShadowMap::Create(int32 resolution)
{
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(resolution)) return nullptr;
    return std::move(shadowMap);
}

void ShadowMap::Bind() const 
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int resolution)
{
    glGenFramebuffers(1, &m_framebuffer);
    return CreateAttachment(resolution);
}

bool ShadowMap::CreateAttachment(int32 resolution)
{
    m_resolution = resolution;

    Bind();
    m_shadowMap = Texture::Create(resolution, resolution, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    m_shadowMap->SetBorderColor(glm::vec4(1.0f));
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 4. 프레임버퍼 완성도 체크
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("Failed to complete shadow map framebuffer: 0x{:X}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    // 기본 FBO로 복귀
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool ShadowMap::Resize(int32 resolution)
{
    if (m_resolution == resolution) return true;
    return CreateAttachment(resolution);
}