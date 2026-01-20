#pragma once
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

public:
    virtual ~CubeTexture();
    static const ResourceType s_ResourceType = ResourceType::CubeTexture;
    virtual ResourceType GetResourceType() const override { return ResourceType::CubeTexture; }
    static CubeTextureUPtr Create(int32 width, int32 height, uint32 format, uint32 type = GL_UNSIGNED_BYTE);
    void GenerateMipmap() const;

private:
    CubeTexture();
    void Init(int32 width, int32 height, uint32 format, uint32 type);
};