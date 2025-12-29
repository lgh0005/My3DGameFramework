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
    MSAAFramebuffer();
    bool Init(int32 width, int32 height, int32 samples);

    uint32 m_msaaFbo                        { 0 };
    uint32 m_msaaDepthStencilBuffer         { 0 };
    uint32 m_samples                        { 0 };
    std::vector<uint32> m_msaaColorBuffers;
};