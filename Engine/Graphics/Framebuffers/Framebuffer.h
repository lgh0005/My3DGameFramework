#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
    virtual ~Framebuffer();

    static void Blit(Framebuffer* src, Framebuffer* dst, uint32 mask, uint32 filter);
    static void BlitRegion
    (
        Framebuffer* src, int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1,
        Framebuffer* dst, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1,
        uint32 mask, uint32 filter
    );
    static void BindToDefault();
    void Bind() const;

    const TexturePtr GetTexture(int32 index = 0) const;
    const TexturePtr GetColorAttachment(int32 index = 0) const;
    const TexturePtr GetDepthAttachment() const;
    void ClearAttachments();

    void AttachColorTexture(uint32 index, TexturePtr texture);
    void AttachDepthTexture(TexturePtr texture);

    const uint32 Get() const { return m_fbo; }
    int32 GetWidth() const { return m_width; }
    int32 GetHeight() const { return m_height; }
    bool OnResize(int32 width, int32 height);

    TexturePtr CreateAndAttachColor
    (
        uint32 index, // 0, 1, 2... (GL_COLOR_ATTACHMENT0 + index)
        uint32 internalFormat,
        uint32 format,
        uint32 type,
        uint32 filter = GL_NEAREST
    );

    TexturePtr CreateAndAttachDepth
    (
        uint32 internalFormat = GL_DEPTH24_STENCIL8,
        uint32 format = GL_DEPTH_STENCIL,
        uint32 type = GL_UNSIGNED_INT_24_8,
        uint32 filter = GL_NEAREST
    );

protected:
    Framebuffer();
    virtual bool CreateAttachments() = 0;
    bool CheckFramebufferStatus(const std::string& name) const;

    // FBO ID
    uint32 m_fbo{ 0 };

    // 공통 속성
    int32 m_width{ 0 };
    int32 m_height{ 0 };

    // 텍스처 관리용 컨테이너
    std::vector<TexturePtr> m_textures;

    // 깊이 버퍼
    TexturePtr m_depthTexture;
};