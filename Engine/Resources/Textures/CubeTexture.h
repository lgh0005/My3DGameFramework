#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Textures/TextureBase.h"
#include "Resources/Textures/TextureUtils.h"
#include <ktx.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(CubeTexture)
class CubeTexture : public TextureBase
{
    friend class TextureUtils;
    DEFINE_RESOURCE_TYPE(ResourceType::CubeTexture, CubeTextureDesc)

public:
    virtual ~CubeTexture();
    static CubeTextureUPtr Create(int32 width, int32 height, uint32 format, uint32 type = GL_UNSIGNED_BYTE);
    void GenerateMipmap() const;

private:
    CubeTexture();
    void Init(int32 width, int32 height, uint32 format, uint32 type);
};