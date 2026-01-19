#pragma once
#include "Framebuffer.h"

CLASS_PTR(MSAAFramebuffer)
class MSAAFramebuffer : public Framebuffer
{
public:
    virtual ~MSAAFramebuffer();
    static MSAAFramebufferUPtr Create(int32 width, int32 height, int32 samples = 4);
    void Bind() const;
    void Resolve() const;

private:
    struct AttachmentInfo
    {
        uint32 internalFormat;
        uint32 format;
        uint32 type;
    };

private:
    MSAAFramebuffer();
    bool Init(int32 width, int32 height, int32 samples);
    virtual bool CreateAttachments() override;

    std::vector<TexturePtr> m_msaaColorTextures;
    TexturePtr m_msaaDepthTexture;

    uint32 m_msaaFbo                        { 0 };
    uint32 m_samples                        { 0 };
    std::vector<AttachmentInfo> m_attachmentInfos;
};