#include "EnginePch.h"
#include "GBufferFramebuffer.h"
#include "Resources/Texture.h"

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
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Attachment 0: Position (RGB16F)
    auto posTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    posTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTexture->Get(), 0);
    m_textures.push_back(std::move(posTexture));

    // Attachment 1: Normal + shininess (RGB16F)
    auto normTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    normTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTexture->Get(), 0);
    m_textures.push_back(std::move(normTexture));

    // Attachment 2: Albedo + Specular (RGBA)
    auto colorTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE);
    colorTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTexture->Get(), 0);
    m_textures.push_back(std::move(colorTexture));

    // Attachment 3 : Emission
    auto emissionTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    emissionTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissionTexture->Get(), 0);
    m_textures.push_back(std::move(emissionTexture));

    // Attachment 4 : velocity
    auto velocityTexture = Texture::Create(width, height, GL_RG16F, GL_RG, GL_FLOAT);
    velocityTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, velocityTexture->Get(), 0);
    m_textures.push_back(std::move(velocityTexture));

    // Draw Buffers 부착
    uint32 attachments[5] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4 
    };
    glDrawBuffers(5, attachments);

    // Depth Texture 사용
    m_depthTexture = Texture::Create(width, height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    m_depthTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    m_depthTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->Get(), 0);

    // Result check
    auto gBufferResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (gBufferResult != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("failed to create gBuffer: {}", gBufferResult);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}