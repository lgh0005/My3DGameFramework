#include "EnginePch.h"
#include "CubeFramebuffer.h"
#include "Resources/Textures/CubeTexture.h"

CubeFramebuffer::CubeFramebuffer() = default;
CubeFramebuffer::~CubeFramebuffer()
{
    CleanDepthStencil();
}

CubeFramebufferUPtr CubeFramebuffer::Create(const CubeTexturePtr colorAttachment, uint32 mipLevel)
{
    auto framebuffer = CubeFramebufferUPtr(new CubeFramebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment, mipLevel))
        return nullptr;
    return std::move(framebuffer);
}

bool CubeFramebuffer::OnResize(int32 width, int32 height)
{
    // 1. 크기가 같으면 패스
    if (m_width == width && m_height == height) return true;

    // 2. 기존 자원 정리
    Super::Clean();
    CleanDepthStencil();

    // 3. 다시 초기화
    return InitWithColorAttachment(m_colorAttachment, m_mipLevel);
}

void CubeFramebuffer::Bind(int32 cubeIndex) const
{
    // 1. FBO 자체를 활성화 (부모의 멤버 m_fbo 사용)
    Super::Bind();

    // 2. 렌더링할 큐브맵의 면(Face)을 교체
    glFramebufferTexture2D
    (
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeIndex,
        m_colorAttachment->Get(),
        m_mipLevel
    );
}

void CubeFramebuffer::CleanDepthStencil()
{
    if (m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
        m_depthStencilBuffer = 0;
    }
}

bool CubeFramebuffer::InitWithColorAttachment(const CubeTexturePtr& colorAttachment, uint32 mipLevel)
{
    m_colorAttachment = colorAttachment;
    m_mipLevel = mipLevel;
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        m_colorAttachment->Get(), m_mipLevel);

    m_width = m_colorAttachment->GetWidth() >> m_mipLevel;
    m_height = m_colorAttachment->GetHeight() >> m_mipLevel;
    if (m_width <= 0) m_width = 1;
    if (m_height <= 0) m_height = 1;

    // 깊이 버퍼 생성
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer
    (
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    if (!CheckFramebufferStatus())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}