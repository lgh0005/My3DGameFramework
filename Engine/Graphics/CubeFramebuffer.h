#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(CubeFramebuffer)
class CubeFramebuffer
{
public:
    static CubeFramebufferUPtr Create(const CubeTexturePtr colorAttachment, uint32 mipLevel = 0);
    static void BindToDefault();
    ~CubeFramebuffer();

    const uint32 Get() const { return m_framebuffer; }
    void Bind(int cubeIndex = 0) const;

    const CubeTexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    CubeFramebuffer() {}
    bool InitWithColorAttachment(const CubeTexturePtr& colorAttachment, uint32 mipLevel);

    uint32 m_framebuffer            { 0 };
    uint32 m_depthStencilBuffer     { 0 };
    uint32 m_mipLevel               { 0 };
    CubeTexturePtr m_colorAttachment;

};