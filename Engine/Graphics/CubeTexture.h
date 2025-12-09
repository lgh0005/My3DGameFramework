#pragma once
#include <ktx.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(CubeTexture)
class CubeTexture
{
public:
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*> images);
    static CubeTextureUPtr CreateFromKtxImage(const std::string& ktxFilePath);
    static CubeTextureUPtr CreateFromKtxHDR(const std::string& ktxFilePath);
    ~CubeTexture();

    const uint32 Get() const { return m_texture; }
    void Bind() const;
private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*> images);
    static CubeTextureUPtr LoadKtx(const std::string& ktxFilePath);

    uint32 m_texture    { 0 };
};