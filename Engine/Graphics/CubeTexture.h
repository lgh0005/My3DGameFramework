#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(CubeTexture)
class CubeTexture
{
public:
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*> images);
    ~CubeTexture();

    const uint32 Get() const { return m_texture; }
    void Bind() const;
private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*> images);
    uint32 m_texture    { 0 };
};