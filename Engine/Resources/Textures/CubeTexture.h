#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Textures/TextureBase.h"
#include <ktx.h>

CLASS_PTR(CubeTexture)
class CubeTexture : public TextureBase
{
    DEFINE_RESOURCE_TYPE(ResourceType::CubeTexture, CubeTextureDesc)

public:
    virtual ~CubeTexture();
    static CubeTexturePtr Load(const CubeTextureDesc& desc);
    static CubeTextureUPtr Create
    (
        int32 width, int32 height, 
        uint32 format, uint32 type = GL_UNSIGNED_BYTE
    );

public:
    virtual CubeTextureDesc& GetDesc() override { return m_desc; }
    virtual const ResourceDesc& GetDesc() const override { return m_desc; }

private:
    CubeTexture();
    
    bool InitFromKtx(const std::string& path);
    void InitProcedural(int32 width, int32 height, uint32 internalFormat, uint32 type);
    void UploadKtxData(struct ktxTexture2* kTexture);

    CubeTextureDesc m_desc;
};