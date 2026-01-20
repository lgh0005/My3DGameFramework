#pragma once
#include "Resources/Resource.h"
#include "Resources/Textures/TextureUtils.h"
#include <ktx.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(CubeTexture)
class CubeTexture : public Resource
{
    friend class TextureUtils;

public:
    virtual ~CubeTexture();
    static const ResourceType s_ResourceType = ResourceType::CubeTexture;
    virtual ResourceType GetResourceType() const override { return ResourceType::CubeTexture; }
    static CubeTextureUPtr Create(int32 width, int32 height, uint32 format, uint32 type = GL_UNSIGNED_BYTE);

    const uint32 Get() const { return m_texture; }
    void Bind() const;
    void GenerateMipmap() const;
    int32 GetWidth() const { return m_width; }
    int32 GetHeight() const { return m_height; }
    uint32 GetFormat() const { return m_format; }
    uint32 GetType() const { return m_type; }

private:
    CubeTexture();
    void Init(int32 width, int32 height, uint32 format, uint32 type);

    uint32 m_texture    { 0 };
    int32  m_width      { 0 };
    int32  m_height     { 0 };
    uint32 m_format     { 0 };
    uint32 m_type       { 0 };
};