#pragma once
#include "Graphics/Framebuffers/FramebufferBase.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer : public FramebufferBase
{
public:
    virtual ~Framebuffer();
    const TexturePtr GetTexture(int32 index = 0) const;
    const TexturePtr GetColorAttachment(int32 index = 0) const;
    const TexturePtr GetDepthAttachment() const;
    void ClearAttachments();

    void AttachColorTexture(uint32 index, TexturePtr texture);
    void AttachDepthTexture(TexturePtr texture);
    TexturePtr CreateAndAttachColor
    (
        uint32 index, uint32 internalFormat, 
        uint32 format, uint32 type, 
        uint32 filter = GL_NEAREST
    );
    TexturePtr CreateAndAttachDepth
    (
        uint32 internalFormat = GL_DEPTH24_STENCIL8,
        uint32 format = GL_DEPTH_STENCIL,
        uint32 type = GL_UNSIGNED_INT_24_8,
        uint32 filter = GL_NEAREST
    );

    virtual bool OnResize(int32 width, int32 height) override;

protected:
    Framebuffer();
    virtual bool CreateAttachments() = 0;

    // 텍스처 관리용 컨테이너
    std::vector<TexturePtr> m_textures;

    // 깊이 버퍼
    TexturePtr m_depthTexture;
};