#include "EnginePch.h"
#include "GBufferFramebuffer.h"
#include "Resources/Texture.h"

GBufferFramebuffer::GBufferFramebuffer() = default;
GBufferFramebuffer::~GBufferFramebuffer() = default;

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

    // Draw Buffers ����
    uint32 attachments[4] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, attachments);

    // Depth Attachment
    glGenRenderbuffers(1, &m_depthBuffer); // ���� ��Ȱ��
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    // �Ϸ� Ȯ��
    auto gBufferResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (gBufferResult != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create gBuffer: {}", gBufferResult); // �α� ���
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}