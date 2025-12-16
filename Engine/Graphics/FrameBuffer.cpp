#include "EnginePch.h"
#include "FrameBuffer.h"
#include "Resources/Texture.h"

FramebufferUPtr Framebuffer::Create(int32 width, int32 height, int32 samples)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->Init(width, height, samples)) return nullptr;
    return std::move(framebuffer);
}

FramebufferUPtr Framebuffer::CreateGBuffer(int32 width, int32 height)
{
    auto gBuffer = FramebufferUPtr(new Framebuffer());
    if (!gBuffer->InitGBuffer(width, height)) return nullptr;
    return std::move(gBuffer);
}

FramebufferUPtr Framebuffer::CreateSSAO(int32 width, int32 height)
{
    auto fbo = FramebufferUPtr(new Framebuffer());
    if (!fbo->InitSSAO(width, height)) return nullptr;
    return std::move(fbo);
}

FramebufferUPtr Framebuffer::CreateBRDFLUT(int32 width, int32 height)
{
    auto fbo = FramebufferUPtr(new Framebuffer());
    if (!fbo->InitBRDFLUT(width, height)) return nullptr;
    return std::move(fbo);
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() 
{
    if (m_msaaDepthStencilBuffer)
        glDeleteRenderbuffers(1, &m_msaaDepthStencilBuffer);
    if (!m_msaaColorBuffers.empty())
        glDeleteRenderbuffers((GLsizei)m_msaaColorBuffers.size(), m_msaaColorBuffers.data());
    if (m_msaaFbo)
        glDeleteFramebuffers(1, &m_msaaFbo);
    if (m_resolveFbo)
        glDeleteFramebuffers(1, &m_resolveFbo);
}

const uint32 Framebuffer::Get() const
{
    if (m_samples > 1) return m_msaaFbo;
    else return m_resolveFbo;
}

void Framebuffer::Bind() const 
{
    if (m_samples > 1)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);
    }
}

void Framebuffer::Resolve() const
{
    if (m_samples <= 1) return;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolveFbo);
    for (int i = 0; i < m_resolveTextures.size(); ++i)
    {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // 상태 복구
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const TexturePtr Framebuffer::GetColorAttachment(int32 index) const
{
    if (index < m_resolveTextures.size()) return m_resolveTextures[index];
    return nullptr;
}

bool Framebuffer::Init(int32 width, int32 height, int32 samples)
{
    m_width = width;
    m_height = height;
    m_samples = samples;

    /*========================================//
    //   샘플 수 1 이하 :  일반 텍스처 FBO 생성 //
    //========================================*/
    if (m_samples <= 1)
    {
        // 1. 일반 FBO 생성 (Resolve FBO 변수 재활용)
        glGenFramebuffers(1, &m_resolveFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

        // 2. 컬러 텍스처 생성 및 부착 (여기서 바로 텍스처에 그립니다)
        const int32 attachmentCount = 2;
        std::vector<uint32> attachments;

        for (int i = 0; i < attachmentCount; ++i)
        {
            // HDR 유지를 위해 GL_RGBA16F 사용
            auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);

            // 필터링은 FXAA 등을 위해 선형(Linear)이 좋을 수 있으나,
            // 픽셀 정확도를 위해 Nearest를 쓰기도 합니다. 취향껏 선택하세요.
            texture->SetFilter(GL_LINEAR, GL_LINEAR);
            texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D, texture->Get(), 0);

            m_resolveTextures.push_back(std::move(texture));
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        // 3. 깊이/스텐실 렌더버퍼 (일반)
        glGenRenderbuffers(1, &m_msaaDepthStencilBuffer); // 변수 재활용
        glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

        // 4. 드로우 버퍼 설정 및 확인
        glDrawBuffers(attachmentCount, attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            SPDLOG_ERROR("FrameBuffer Init Failed!");
            return false;
        }

        // m_msaaFbo는 사용 안 함 : 0으로 초기화
        m_msaaFbo = 0;
    }

    /*================================//
    //   샘플 수 2 이상 :  MSAA 수행   //
    //================================*/
    else
    {
        glGenFramebuffers(1, &m_msaaFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);

        const int32 attachmentCount = 2;
        std::vector<uint32> attachments;
        for (int i = 0; i < attachmentCount; ++i)
        {
            uint32 colorBuffer;
            glGenRenderbuffers(1, &colorBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA16F, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, colorBuffer);
            m_msaaColorBuffers.push_back(colorBuffer);
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        glGenRenderbuffers(1, &m_msaaDepthStencilBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

        glDrawBuffers(attachmentCount, attachments.data());
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        // Resolve FBO 생성 부분
        glGenFramebuffers(1, &m_resolveFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

        for (int i = 0; i < attachmentCount; ++i)
        {
            // 여기도 MSAA 결과를 받을 때 GL_RGBA16F로 받아야 HDR이 유지됩니다.
            auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->Get(), 0);
            m_resolveTextures.push_back(std::move(texture));
        }

        glDrawBuffers(attachmentCount, attachments.data());
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;
    }

    BindToDefault();
    return true;
}

bool Framebuffer::InitGBuffer(int32 width, int32 height)
{
    m_width = width;
    m_height = height;
    m_samples = 1;
    
    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    // Attachment 0: Position (RGB16F)
    auto posTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    posTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(posTexture));

    // Attachment 1: Normal + shininess (RGB16F)
    auto normTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    normTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(normTexture));

    // Attachment 2: Albedo + Specular (RGBA)
    auto colorTexture = Texture::Create(width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    colorTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(colorTexture));

    // Attachment 3 : Emission
    auto emissionTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    emissionTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissionTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(emissionTexture));

    // Draw Buffers 설정
    uint32 attachments[4] = 
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, attachments);

    // Depth Attachment
    glGenRenderbuffers(1, &m_msaaDepthStencilBuffer); // 변수 재활용
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

    // 완료 확인
    auto gBufferResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (gBufferResult != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create gBuffer: {}", gBufferResult); // 로그 출력
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool Framebuffer::InitSSAO(int32 width, int32 height)
{
    m_width = width;
    m_height = height;
    m_samples = 1;

    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    // SSAO는 GL_RED 채널만 있으면 충분합니다 (메모리 절약)
    // 정밀도를 위해 GL_R16F 혹은 GL_RED 사용
    auto texture = Texture::Create(width, height, GL_R16F, GL_RED, GL_FLOAT);
    texture->SetFilter(GL_NEAREST, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);
    m_resolveTextures.push_back(std::move(texture));

    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool Framebuffer::InitBRDFLUT(int32 width, int32 height)
{
    m_width = width;
    m_height = height;
    m_samples = 1;

    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    auto texture = Texture::Create(width, height, GL_RG16F, GL_RG, GL_FLOAT);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);
    m_resolveTextures.push_back(std::move(texture));

    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("BRDF LUT Framebuffer Incomplete!");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}
