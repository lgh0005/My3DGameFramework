#include "EnginePch.h"
#include "CubeFramebuffer.h"
#include "Resources/CubeTexture.h"

CubeFramebuffer::CubeFramebuffer() = default;
CubeFramebuffer::~CubeFramebuffer()
{
    if (m_depthStencilBuffer) glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    if (m_framebuffer) glDeleteFramebuffers(1, &m_framebuffer);
}

CubeFramebufferUPtr CubeFramebuffer::Create(const CubeTexturePtr colorAttachment, uint32 mipLevel)
{
    auto framebuffer = CubeFramebufferUPtr(new CubeFramebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment, mipLevel))
        return nullptr;
    return std::move(framebuffer);
}

void CubeFramebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeFramebuffer::Bind(int32 cubeIndex) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // 타겟 텍스쳐의 특정 밉맵 레벨에 그립니다.
    glFramebufferTexture2D
    (
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeIndex,
        m_colorAttachment->Get(),
        m_mipLevel
    );

    // TODO : 밉맵 레벨이 바뀌면 Viewport 크기도 그에 맞춰 줄여줘야 합니다.
    // 이는 이 함수를 호출하는 쪽(Baking Loop)에서 glViewport로 처리해야 합니다.
    // IBL Specular를 위해 필요
    // (미리보기) 나중에 작성할 로직
    //for (int mip = 0; mip < maxMipLevels; ++mip)
    //{
    //    // 밉맵 레벨에 따라 뷰포트 크기를 줄임 (ex: 128 -> 64 -> 32 ...)
    //    unsigned int mipWidth = width * std::pow(0.5, mip);
    //    unsigned int mipHeight = height * std::pow(0.5, mip);

    //    // [중요] FBO 바인딩 전/후에 뷰포트 조절 필수
    //    glViewport(0, 0, mipWidth, mipHeight);

    //    for (int i = 0; i < 6; ++i)
    //    {
    //        framebuffer->Bind(i, mip); // 여기서 바인딩
    //        // ... 그리기 ...
    //    }
    //}
}

bool CubeFramebuffer::InitWithColorAttachment(const CubeTexturePtr& colorAttachment, uint32 mipLevel)
{
    m_colorAttachment = colorAttachment;
    m_mipLevel = mipLevel;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        m_colorAttachment->Get(), m_mipLevel);

    int32 width = m_colorAttachment->GetWidth() >> m_mipLevel;
    int32 height = m_colorAttachment->GetHeight() >> m_mipLevel;

    // 깊이 버퍼 생성
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("failed to create framebuffer: 0x{:04x}", result);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}