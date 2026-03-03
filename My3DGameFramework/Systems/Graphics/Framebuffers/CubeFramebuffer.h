#pragma once
#include "Graphics/Framebuffers/FramebufferBase.h"

namespace MGF3D
{
    CLASS_PTR(CubeTexture)


    CLASS_PTR(CubeFramebuffer)
    class CubeFramebuffer : public FramebufferBase
    {
        using Super = FramebufferBase;

    public:
        ~CubeFramebuffer();
        static CubeFramebufferUPtr Create(const CubeTexturePtr colorAttachment, uint32 mipLevel = 0);
        virtual bool OnResize(int32 width, int32 height) override;
        const CubeTexturePtr GetColorAttachment() const { return m_colorAttachment; }
        void Bind(int32 cubeIndex) const;

        void CleanDepthStencil();

    private:
        CubeFramebuffer();
        bool InitWithColorAttachment(const CubeTexturePtr& colorAttachment, uint32 mipLevel);

        uint32 m_depthStencilBuffer{ 0 };
        uint32 m_mipLevel{ 0 };
        CubeTexturePtr m_colorAttachment;
    };
}
