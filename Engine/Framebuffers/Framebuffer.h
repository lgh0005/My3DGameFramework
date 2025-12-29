#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
    virtual ~Framebuffer();

    static void BindToDefault();
    void Bind() const;

    const TexturePtr GetColorAttachment(int32 index = 0) const;
    const TexturePtr GetTexture(int32 index = 0) const;
    const uint32 Get() const { return m_fbo; }

    int32 GetWidth() const { return m_width; }
    int32 GetHeight() const { return m_height; }

protected:
    Framebuffer();

    // FBO ID
    uint32 m_fbo{ 0 };

    // 공통 속성
    int32 m_width{ 0 };
    int32 m_height{ 0 };

    // 텍스처 관리용 컨테이너
    std::vector<TexturePtr> m_textures;

    // 깊이 버퍼
    uint32 m_depthBuffer{ 0 };
};