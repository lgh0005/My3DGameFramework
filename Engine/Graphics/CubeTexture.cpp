#include "EnginePch.h"
#include "CubeTexture.h"
#include "Graphics/Image.h"

/*=========================================//
//   default cube texture create methods   //
//=========================================*/
CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> images) 
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images)) return nullptr;
    return std::move(texture);
}

CubeTextureUPtr CubeTexture::CreateFromKtxImage(const std::string& ktxFilePath)
{
    return LoadKtx(ktxFilePath);
}

CubeTextureUPtr CubeTexture::CreateFromKtxHDR(const std::string& ktxFilePath)
{
    return LoadKtx(ktxFilePath);
}

/*==================================//
//   default cube texture setters   //
//==================================*/
CubeTexture::~CubeTexture() 
{
    if (m_texture) 
        glDeleteTextures(1, &m_texture);
}

void CubeTexture::Bind() const 
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

/*========================//
//   cube texture utils   //
//========================*/
bool CubeTexture::InitFromImages(const std::vector<Image*> images) 
{
    glGenTextures(1, &m_texture);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 정렬 문제 해결 (이거 없으면 RGB 텍스쳐 깨짐)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (uint32 i = 0; i < (uint32)images.size(); i++) 
    {
        auto image = images[i];
        
        GLenum format = GL_RGBA;
        GLenum internalFormat = GL_RGBA; // GPU에 저장될 포맷
        GLenum type = GL_UNSIGNED_BYTE;  // CPU 데이터 타입

        // 1. 채널 수 확인
        switch (image->GetChannelCount()) 
        {
            case 1: format = GL_RED; break;
            case 2: format = GL_RG; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: break;
        }

        // 2. 데이터 타입 확인 (LDR vs HDR 분기 처리)
        if (image->GetBytePerChannel() == 4) // HDR (Float)
        {
            type = GL_FLOAT;
            if (format == GL_RGB) internalFormat = GL_RGB16F;
            else if (format == GL_RGBA) internalFormat = GL_RGBA16F;
        }
        else // LDR (Byte)
        {
            type = GL_UNSIGNED_BYTE;
            internalFormat = format; // RGB8, RGBA8 등
        }

        glTexImage2D
        (
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
            image->GetWidth(), image->GetHeight(), 0,
            format, type,
            image->GetData()
        );
    }

    return true;
}

CubeTextureUPtr CubeTexture::LoadKtx(const std::string& ktxFilePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;
    GLuint textureID = 0;
    GLenum target;
    GLenum glerror;

    // 1. KTX 파일 로드
    result = ktxTexture_CreateFromNamedFile
    (
        ktxFilePath.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );
    if (result != KTX_SUCCESS)
    {
        SPDLOG_ERROR("Failed to load KTX cubemap: {}", ktxFilePath);
        return nullptr;
    }

    // 2. 큐브맵 검증
    if (!kTexture->isCubemap)
    {
        SPDLOG_ERROR("Texture is not a cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    // 3. GPU 업로드
    result = ktxTexture_GLUpload(kTexture, &textureID, &target, &glerror);
    if (result != KTX_SUCCESS)
    {
        SPDLOG_ERROR("Failed to upload KTX cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->m_texture = textureID;

    // 4. 공통 파라미터 설정
    // 큐브맵은 HDR이든 아니든 모서리 솔기(Seam) 방지를 위해 무조건 Clamp To Edge.
    texture->Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    ktxTexture_Destroy(kTexture);

    return std::move(texture);
}