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

FramebufferUPtr Framebuffer::CreatePostProcess(int32 width, int32 height, bool useDepth)
{
    auto fbo = FramebufferUPtr(new Framebuffer());
    if (!fbo->InitPostProcess(width, height, useDepth)) return nullptr;
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

    // ���� ����
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AttachTexture(Texture* texture, int32 attachmentIndex)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex,
        GL_TEXTURE_2D, texture->Get(), 0);
    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 + (uint32)attachmentIndex };
    glDrawBuffers(1, attachments);
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
    //   ���� �� 1 ���� :  �Ϲ� �ؽ�ó FBO ���� //
    //========================================*/
    if (m_samples <= 1)
    {
        // 1. �Ϲ� FBO ���� (Resolve FBO ���� ��Ȱ��)
        glGenFramebuffers(1, &m_resolveFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

        // 2. �÷� �ؽ�ó ���� �� ���� (���⼭ �ٷ� �ؽ�ó�� �׸��ϴ�)
        const int32 attachmentCount = 2;
        std::vector<uint32> attachments;

        for (int i = 0; i < attachmentCount; ++i)
        {
            // HDR ������ ���� GL_RGBA16F ���
            auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);

            // ���͸��� FXAA ���� ���� ����(Linear)�� ���� �� ������,
            // �ȼ� ��Ȯ���� ���� Nearest�� ���⵵ �մϴ�. ���ⲯ �����ϼ���.
            texture->SetFilter(GL_LINEAR, GL_LINEAR);
            texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D, texture->Get(), 0);

            m_resolveTextures.push_back(std::move(texture));
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        // 3. ����/���ٽ� �������� (�Ϲ�)
        glGenRenderbuffers(1, &m_msaaDepthStencilBuffer); // ���� ��Ȱ��
        glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

        // 4. ��ο� ���� ���� �� Ȯ��
        glDrawBuffers(attachmentCount, attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            SPDLOG_ERROR("FrameBuffer Init Failed!");
            return false;
        }

        // m_msaaFbo�� ��� �� �� : 0���� �ʱ�ȭ
        m_msaaFbo = 0;
    }

    /*================================//
    //   ���� �� 2 �̻� :  MSAA ����   //
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

        // Resolve FBO ���� �κ�
        glGenFramebuffers(1, &m_resolveFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

        for (int i = 0; i < attachmentCount; ++i)
        {
            // ���⵵ MSAA ����� ���� �� GL_RGBA16F�� �޾ƾ� HDR�� �����˴ϴ�.
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
    auto colorTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE);
    colorTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(colorTexture));

    // Attachment 3 : Emission
    auto emissionTexture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    emissionTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissionTexture->Get(), 0);
    m_resolveTextures.push_back(std::move(emissionTexture));

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
    glGenRenderbuffers(1, &m_msaaDepthStencilBuffer); // ���� ��Ȱ��
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);

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

bool Framebuffer::InitSSAO(int32 width, int32 height)
{
    m_width = width;
    m_height = height;
    m_samples = 1;

    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    // SSAO�� GL_RED ä�θ� ������ ����մϴ� (�޸� ����)
    // ���е��� ���� GL_R16F Ȥ�� GL_RED ���
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

bool Framebuffer::InitPostProcess(int32 width, int32 height, bool useDepth)
{
    m_width = width;
    m_height = height;
    m_samples = 1;

    glGenFramebuffers(1, &m_resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFbo);

    // 1. �÷� ÷�ι� (1���� ����!)
    // HDR�� ���� GL_RGBA16F ���
    auto texture = Texture::Create(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Get(), 0);
    m_resolveTextures.push_back(std::move(texture));

    // 2. Draw Buffer ���� (0�� �ϳ���!)
    uint32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // 3. (����) ���� ���۰� �ʿ��ϴٸ� �߰� (BlitCopyDepth�� ���� �ʿ��� �� ����)
    // ���� PostProcess ��ü�� ���� �׽�Ʈ�� ���� �׸�����, 
    // Forward Rendering�� ���� ���̸� �����ؿ� ������ �ʿ��ϴٸ� �����ؾ� ��.
    // 2. ���� ���� ���� (�ɼ� ó��!)
    if (useDepth) // <--- ���Ⱑ �ٽ� ������
    {
        glGenRenderbuffers(1, &m_msaaDepthStencilBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthStencilBuffer);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}